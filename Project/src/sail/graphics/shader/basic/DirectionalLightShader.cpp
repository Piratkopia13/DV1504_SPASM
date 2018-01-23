#include "DirectionalLightShader.h"

using namespace DirectX::SimpleMath;

D3D11_INPUT_ELEMENT_DESC DirectionalLightShader::IED[2] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

DirectionalLightShader::DirectionalLightShader()
	: m_vpMatrix(Matrix::Identity)
{

	ModelDataBuffer defaultModelBuffer = { Vector4::One, Matrix::Identity, Matrix::Identity };
	m_transformBuffer = std::unique_ptr<ShaderComponent::ConstantBuffer>(new ShaderComponent::ConstantBuffer(&defaultModelBuffer, sizeof(ModelDataBuffer)));
	ClippingPlaneBuffer defaultClippingBuffer = { Vector4(0.f, 0.f, 0.f, 0.f) };
	m_clippingPlaneBuffer = std::unique_ptr<ShaderComponent::ConstantBuffer>(new ShaderComponent::ConstantBuffer(&defaultClippingBuffer, sizeof(ClippingPlaneBuffer)));

	// Compile shader and add to this shaderSet
	auto vsBlob = compileShader(L"DirectionalLightShader.hlsl", "VSMain", "vs_5_0");
	setVertexShader(vsBlob);

	// Compile shader and add to this shaderSet
	auto psBlob = compileShader(L"DirectionalLightShader.hlsl", "PSMain", "ps_5_0");
	setPixelShader(psBlob);

	// Create the input layout
	Application::getInstance()->getDXManager()->getDevice()->CreateInputLayout(IED, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);

	// Done with the blobs, release them
	Memory::safeRelease(vsBlob);
	Memory::safeRelease(psBlob);

}
DirectionalLightShader::~DirectionalLightShader() {
	Memory::safeRelease(m_inputLayout);
}

void DirectionalLightShader::updateCamera(Camera& cam) {
	m_vpMatrix = cam.getViewProjection();
}

void DirectionalLightShader::updateBuffer(const Vector4& color, const Matrix& w, const Matrix& vp) const {
	ModelDataBuffer data = { color, w.Transpose(), vp.Transpose() };
	m_transformBuffer->updateData(&data, sizeof(data));
}

void DirectionalLightShader::setClippingPlane(const DirectX::SimpleMath::Vector4& clippingPlane) {
	ClippingPlaneBuffer data = { clippingPlane };
	m_clippingPlaneBuffer->updateData(&data, sizeof(data));
}

void DirectionalLightShader::bind() {
	ShaderSet::bind();

	// Set input layout as active
	Application::getInstance()->getDXManager()->getDeviceContext()->IASetInputLayout(m_inputLayout);

	m_transformBuffer->bind(ShaderComponent::VS, 0);
	m_clippingPlaneBuffer->bind(ShaderComponent::VS, 1);

}

void DirectionalLightShader::createBufferFromModelData(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer, const void* data) {

	Model::Data modelData = *(Model::Data*)data;

	if (modelData.numVertices <= 0 || !modelData.positions)
		Logger::Error("numVertices or position data not set for model");

	if (!modelData.normals) {
		//Logger::Warning("Normals not set for model");
	}

	// Create the vertex array that this shader uses
	DirectionalLightShader::Vertex* vertices = new DirectionalLightShader::Vertex[modelData.numVertices];

	// Fill the array with vertex data
	for (UINT i = 0; i < modelData.numVertices; i++) {
		vertices[i].position = modelData.positions[i];
		if (modelData.normals)
			vertices[i].normal = modelData.normals[i];
		else
			vertices[i].normal = Vector3(0.f, 1.f, 0.f);
	}

	// Set up vertex buffer description
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = modelData.numVertices * sizeof(DirectionalLightShader::Vertex);
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

void DirectionalLightShader::draw(Model& model, bool bindFirst) {

	if (bindFirst) {
		// Bind the shaders
		// Bind the input layout
		// bind constant buffer
		bind();
	}

	// Update the world matrix to match this model
	auto modelColor = model.getMaterial()->getColor();
	updateBuffer(modelColor, model.getTransform().getMatrix(), m_vpMatrix);

	// Bind vertex buffer
	UINT stride = sizeof(DirectionalLightShader::Vertex);
	UINT offset = 0;
	Application::getInstance()->getDXManager()->getDeviceContext()->IASetVertexBuffers(0, 1, model.getVertexBuffer(), &stride, &offset);

	// Bind index buffer if one exists
	auto* iBuffer = model.getIndexBuffer();
	if (iBuffer)
		Application::getInstance()->getDXManager()->getDeviceContext()->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set topology
	Application::getInstance()->getDXManager()->getDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw
	if (iBuffer)
		Application::getInstance()->getDXManager()->getDeviceContext()->DrawIndexed(model.getNumIndices(), 0, 0);
	else
		Application::getInstance()->getDXManager()->getDeviceContext()->Draw(model.getNumVertices(), 0);

}
