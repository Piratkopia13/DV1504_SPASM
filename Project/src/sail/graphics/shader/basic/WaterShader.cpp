#include "WaterShader.h"

using namespace DirectX::SimpleMath;

const float WaterShader::WAVE_SPEED = 0.02f;

D3D11_INPUT_ELEMENT_DESC WaterShader::IED[2] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

WaterShader::WaterShader() {

	ModelDataBuffer defaultBuffer = { Matrix::Identity, Matrix::Identity };
	m_transformBuffer = std::unique_ptr<ShaderComponent::ConstantBuffer>(new ShaderComponent::ConstantBuffer(&defaultBuffer, sizeof(ModelDataBuffer)));
	WaterStuffBuffer defaultBuffer2 = { 0.f, 0.f, 0.f, 0.f };
	m_waterBuffer = std::unique_ptr<ShaderComponent::ConstantBuffer>(new ShaderComponent::ConstantBuffer(&defaultBuffer2, sizeof(WaterStuffBuffer)));
	CameraBuffer defaultBuffer3 = { Vector3::Zero };
	m_cameraBuffer = std::unique_ptr<ShaderComponent::ConstantBuffer>(new ShaderComponent::ConstantBuffer(&defaultBuffer3, sizeof(CameraBuffer)));
	LightBuffer defaultBuffer4 = {};
	m_lightBuffer = std::unique_ptr<ShaderComponent::ConstantBuffer>(new ShaderComponent::ConstantBuffer(&defaultBuffer4, sizeof(LightBuffer)));
	m_sampler = std::make_unique<ShaderComponent::Sampler>();

	// Compile shader and add to this shaderSet
	auto vsBlob = compileShader(L"WaterShader.hlsl", "VSMain", "vs_5_0");
	setVertexShader(vsBlob);

	// Compile shader and add to this shaderSet
	auto psBlob = compileShader(L"WaterShader.hlsl", "PSMain", "ps_5_0");
	setPixelShader(psBlob);

	// Create the inputlayout
	Application::getInstance()->getDXManager()->getDevice()->CreateInputLayout(IED, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);

	// Done with the blobs, release them
	Memory::safeRelease(vsBlob);
	Memory::safeRelease(psBlob);

}
WaterShader::~WaterShader() {
	Memory::safeRelease(m_inputLayout);
}

void WaterShader::updateCamera(Camera& cam, bool waterReflection) {
	m_vpMatrix = cam.getViewProjection();
	m_camPos = cam.getPosition();
	m_camPosHasChanged = true;
}

void WaterShader::updateWaves(float dt) {
	m_dudvMoveAmount += WAVE_SPEED * dt;
	m_dudvMoveAmount = fmod(m_dudvMoveAmount, 1.f);
	//if (m_dudvMoveAmount > 1.f) m_dudvMoveAmount -= 1.f;
}

void WaterShader::updateBuffer(const Matrix& w, const Matrix& vp) const {
	ModelDataBuffer data = { w.Transpose(), vp.Transpose() };
	m_transformBuffer->updateData(&data, sizeof(data));

	WaterStuffBuffer waterData = { m_dudvMoveAmount };
	m_waterBuffer->updateData(&waterData, sizeof(waterData));
}

void WaterShader::updateCameraBuffer() {
	CameraBuffer data = { m_camPos };
	m_cameraBuffer->updateData(&data, sizeof(data));
}

void WaterShader::updateLights(const Lights& lights) {
	LightBuffer data = { lights.getDL() };
	m_lightBuffer->updateData(&data, sizeof(data));
}

void WaterShader::bind() {
	ShaderSet::bind();

	// Set input layout as active
	Application::getInstance()->getDXManager()->getDeviceContext()->IASetInputLayout(m_inputLayout);

	// Bind the transform constant buffer
	m_transformBuffer->bind(ShaderComponent::VS, 0);
	// Bind the camera constant buffer
	m_cameraBuffer->bind(ShaderComponent::VS, 1);
	// Bind the water constant buffer
	m_waterBuffer->bind(ShaderComponent::PS, 0);
	// Bind the light constant buffer
	m_lightBuffer->bind(ShaderComponent::PS, 1);
	// Bind sampler
	m_sampler->bind();

}

void WaterShader::createBufferFromModelData(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer, const void* data) {

	Model::Data modelData = *(Model::Data*)data;

	if (modelData.numVertices <= 0 || !modelData.positions)
		Logger::Error("numVertices or position data not set for model");

	if (!modelData.texCoords) {
		Logger::Warning("Texture coordinates not set for model that will render with a texture shader");
	}

	// Create the vertex array that this shader uses
	WaterShader::Vertex* vertices = new WaterShader::Vertex[modelData.numVertices];

	for (UINT i = 0; i < modelData.numVertices; i++) {
		vertices[i].position = modelData.positions[i];
		if (modelData.texCoords)
			vertices[i].texCoord = modelData.texCoords[i];
		else
			vertices[i].texCoord = Vector2(0.f, 0.f);
	}

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));

	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = modelData.numVertices * sizeof(WaterShader::Vertex);
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

void WaterShader::draw(Model& model, bool bindFirst) {

	if (bindFirst) {
		// Bind the shaders
		// Bind the input layout
		// bind constant buffer
		bind();
	}

	// Update the world matrix to match this model
	updateBuffer(model.getTransform().getMatrix(), m_vpMatrix);

	if (m_camPosHasChanged) {
		updateCameraBuffer();
		m_camPosHasChanged = false;
	}

	// Bind the texture if it exists
	UINT numTextures;
	auto tex = model.getMaterial()->getTextures(numTextures);
	if (tex)
		Application::getInstance()->getDXManager()->getDeviceContext()->PSSetShaderResources(0, numTextures, tex);

	// Bind vertex buffer
	UINT stride = sizeof(WaterShader::Vertex);
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

	ID3D11ShaderResourceView* nullSRV[4] = { nullptr, nullptr, nullptr, nullptr };
	Application::getInstance()->getDXManager()->getDeviceContext()->PSSetShaderResources(0, numTextures, nullSRV);

}