#include "PostProcessFlushShader.h"

using namespace DirectX;
using namespace SimpleMath;

D3D11_INPUT_ELEMENT_DESC PostProcessFlushShader::IED[1] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

PostProcessFlushShader::PostProcessFlushShader() {

	// Set up sampler for point sampling
	m_sampler = std::make_unique<ShaderComponent::Sampler>(D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	// Compile VS
	auto vsBlob = compileShader(L"postprocess/PostProcessFlushShader.hlsl", "VSMain", "vs_5_0");
	// Add the VS to this ShaderSet
	setVertexShader(vsBlob);

	// Compile PS
	auto psBlob = compileShader(L"postprocess/PostProcessFlushShader.hlsl", "PSMain", "ps_5_0");
	// Add the PS to this ShaderSet
	setPixelShader(psBlob);

	// Create the input layout
	Application::getInstance()->getDXManager()->getDevice()->CreateInputLayout(IED, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);

	// Done with the blobs, release them
	Memory::safeRelease(vsBlob);
	Memory::safeRelease(psBlob);

}
PostProcessFlushShader::~PostProcessFlushShader() {
	Memory::safeRelease(m_inputLayout);
}

void PostProcessFlushShader::bind() {

	// Call parent to bind shaders
	ShaderSet::bind();

	// Set input layout as active
	Application::getInstance()->getDXManager()->getDeviceContext()->IASetInputLayout(m_inputLayout);

	// Bind sampler
	m_sampler->bind();

}

void PostProcessFlushShader::createBufferFromModelData(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer, ID3D11Buffer** instanceBuffer, const void* data) {

	Model::Data& modelData = *(Model::Data*)data;

	if (modelData.numVertices <= 0 || !modelData.positions)
		Logger::Error("numVertices or position data not set for model");

	if (!modelData.texCoords)
		Logger::Warning("Texture coordinates not set for model that will render with a texture shader");

	// Create the vertex array that this shader uses
	PostProcessFlushShader::Vertex* vertices = new PostProcessFlushShader::Vertex[modelData.numVertices];

	for (UINT i = 0; i < modelData.numVertices; i++) {
		// Position
		vertices[i].position = modelData.positions[i];
	}

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = modelData.numVertices * sizeof(PostProcessFlushShader::Vertex);
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

void PostProcessFlushShader::draw(Model& model, bool bindFirst) {

	if (bindFirst)
		bind();

	auto devCon = Application::getInstance()->getDXManager()->getDeviceContext();

	// Bind the texture if it exists
	UINT numTextures;
	auto tex = model.getMaterial()->getTextures(numTextures);
	if (tex)
		devCon->PSSetShaderResources(0, numTextures, tex);

	// Bind vertex buffer
	UINT stride = sizeof(PostProcessFlushShader::Vertex);
	UINT offset = 0;
	devCon->IASetVertexBuffers(0, 1, model.getVertexBuffer(), &stride, &offset);

	// Bind index buffer if one exitsts
	auto iBuffer = model.getIndexBuffer();
	if (iBuffer)
		devCon->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set topology
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	if (iBuffer)
		devCon->DrawIndexed(model.getNumIndices(), 0, 0);
	else
		devCon->Draw(model.getNumVertices(), 0);

	ID3D11ShaderResourceView* nullSRV[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	Application::getInstance()->getDXManager()->getDeviceContext()->PSSetShaderResources(0, 5, nullSRV);

}
