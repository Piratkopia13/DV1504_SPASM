#include "TessellationTestShader.h"

using namespace DirectX;
using namespace SimpleMath;

D3D11_INPUT_ELEMENT_DESC TessellationTestShader::IED[3] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

TessellationTestShader::TessellationTestShader() {

	// Set up constant buffers
	ModelDataBuffer defaultModelData = { Matrix::Identity, Matrix::Identity };
	m_modelDataBuffer = std::unique_ptr<ShaderComponent::ConstantBuffer>(new ShaderComponent::ConstantBuffer(&defaultModelData, sizeof(ModelDataBuffer)));

	// Set up sampler
	m_sampler = std::make_unique<ShaderComponent::Sampler>();

	// Compile VS
	auto vsBlob = compileShader(L"tessellation/Vertex.hlsl", "VSMain", "vs_5_0");
	// Add the VS to this ShaderSet
	setVertexShader(vsBlob);

	// Compile HS
	auto hsBlob = compileShader(L"tessellation/Hull.hlsl", "HSMain", "hs_5_0");
	// Add the HS to this ShaderSet
	setHullShader(hsBlob);

	// Compile DS
	auto dsBlob = compileShader(L"tessellation/Domain.hlsl", "DSMain", "ds_5_0");
	// Add the DS to this ShaderSet
	setDomainShader(dsBlob);

	// Compile GS
	auto gsBlob = compileShader(L"tessellation/Geometry.hlsl", "GSMain", "gs_5_0");
	// Add the GS to this ShaderSet
	setGeometryShader(gsBlob);

	// Compile PS
	auto psBlob = compileShader(L"tessellation/Pixel.hlsl", "PSMain", "ps_5_0");
	// Add the PS to this ShaderSet
	setPixelShader(psBlob);

	// Create the input layout
	Application::getInstance()->getDXManager()->getDevice()->CreateInputLayout(IED, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);

	// Done with the blobs, release them
	Memory::safeRelease(vsBlob);
	Memory::safeRelease(psBlob);
	Memory::safeRelease(gsBlob);
	Memory::safeRelease(dsBlob);
	Memory::safeRelease(hsBlob);

}
TessellationTestShader::~TessellationTestShader() {
	Memory::safeRelease(m_inputLayout);
}

void TessellationTestShader::updateCamera(Camera& cam, bool waterReflection) {
	m_mVP = cam.getViewProjection();
}

void TessellationTestShader::updateModelDataBuffer(const DirectX::SimpleMath::Matrix& w, const DirectX::SimpleMath::Matrix& vp) const {

	ModelDataBuffer data = {
		w.Transpose(), vp.Transpose()
	};

	m_modelDataBuffer->updateData(&data, sizeof(data));
}

void TessellationTestShader::bind() {

	// Call parent to bind shaders
	ShaderSet::bind();

	// Set input layout as active
	Application::getInstance()->getDXManager()->getDeviceContext()->IASetInputLayout(m_inputLayout);

	// Bind cbuffers
	m_modelDataBuffer->bind(ShaderComponent::DS, 0);
	// Bind sampler
	m_sampler->bind(ShaderComponent::DS);
	m_sampler->bind(ShaderComponent::PS);

}

void TessellationTestShader::createBufferFromModelData(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer, const void* data) {

	Model::Data modelData = *(Model::Data*)data;

	if (modelData.numVertices <= 0 || !modelData.positions)
		Logger::Error("numVertices or position data not set for model");

	if (!modelData.texCoords)
		Logger::Warning("Texture coordinates not set for model that will render with a texture shader");

	// Create the vertex array that this shader uses
	TessellationTestShader::Vertex* vertices = new TessellationTestShader::Vertex[modelData.numVertices];

	for (UINT i = 0; i < modelData.numVertices; i++) {
		// Position
		vertices[i].position = modelData.positions[i];
		// Tex coords
		if (modelData.texCoords)
			vertices[i].texCoords = modelData.texCoords[i];
		else
			vertices[i].texCoords = Vector2::Zero;
		// Normals
		if (modelData.normals)
			vertices[i].normal = modelData.normals[i];
		else
			vertices[i].normal = Vector3::Zero;
	}

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = modelData.numVertices * sizeof(TessellationTestShader::Vertex);
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = vertices;

	// Create the vertex buffer
	ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateBuffer(&vbd, &vertexData, vertexBuffer));
	// Delete vertices from cpu memory
	Memory::safeDeleteArr(vertices);

	// Set up index buffer if indices are set
	if (modelData.numIndices > 0) {

		ULONG* indices = new ULONG[modelData.numIndices];

		// Fill the array with the model indices
		for (UINT i = 0; i < modelData.numIndices; i++) {
			indices[i] = modelData.indices[i];
		}

		// Set up index buffer description
		D3D11_BUFFER_DESC ibd;
		ZeroMemory(&ibd, sizeof(ibd));
		ibd.Usage = D3D11_USAGE_IMMUTABLE;
		ibd.ByteWidth = modelData.numIndices * sizeof(UINT);
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA indexData;
		ZeroMemory(&indexData, sizeof(indexData));
		indexData.pSysMem = indices;

		// Create the index buffer
		ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateBuffer(&ibd, &indexData, indexBuffer));
		// Delete indices from cpu memory
		Memory::safeDeleteArr(indices);
	}

}

void TessellationTestShader::draw(Model& model, bool bindFirst) {

	if (bindFirst)
		bind();

	// Update the model data to match this model
	updateModelDataBuffer(model.getTransform().getMatrix(), m_mVP);

	auto devCon = Application::getInstance()->getDXManager()->getDeviceContext();

	// Bind the texture if it exists
	UINT numTextures;
	auto tex = model.getMaterial()->getTextures(numTextures);
	if (tex) {
		devCon->PSSetShaderResources(0, numTextures, tex);
		devCon->DSSetShaderResources(0, 1, &tex[1]);
	}

	// Bind vertex buffer
	UINT stride = sizeof(TessellationTestShader::Vertex);
	UINT offset = 0;
	devCon->IASetVertexBuffers(0, 1, model.getVertexBuffer(), &stride, &offset);

	// Bind index buffer if one exitsts
	auto iBuffer = model.getIndexBuffer();
	if (iBuffer)
		devCon->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set topology
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	// Draw
	if (iBuffer)
		devCon->DrawIndexed(model.getNumIndices(), 0, 0);
	else
		devCon->Draw(model.getNumVertices(), 0);

	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	Application::getInstance()->getDXManager()->getDeviceContext()->PSSetShaderResources(0, 1, nullSRV);

}
