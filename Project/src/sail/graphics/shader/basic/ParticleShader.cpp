#include "ParticleShader.h"


using namespace DirectX::SimpleMath;

ParticleShader::ParticleShader() {

	m_maxNumParticles = 1000000;

	// Loads the buffers with some default parameters
	CameraDataBuffer defaultCameraBuffer = { Vector3::One, 0.f, Vector3::One };
	m_cameraDataBuffer = std::unique_ptr<ShaderComponent::ConstantBuffer>(new ShaderComponent::ConstantBuffer(&defaultCameraBuffer, sizeof(CameraDataBuffer)));
	ModelDataBuffer defaultModelBuffer = { Matrix::Identity };
	m_modelDataBuffer = std::unique_ptr<ShaderComponent::ConstantBuffer>(new ShaderComponent::ConstantBuffer(&defaultModelBuffer, sizeof(ModelDataBuffer)));
	ParticleParametersBuffer defaultPartParBuffer = { Vector3::One, 0.f, Vector3::One };
	m_particleParameterBuffer = std::unique_ptr<ShaderComponent::ConstantBuffer>(new ShaderComponent::ConstantBuffer(&defaultPartParBuffer, sizeof(ParticleParametersBuffer)));
	ParticleCountBuffer defaultPartCountBuffer = { { 0, 0, 0, 0 } };
	m_particleCountBuffer = std::unique_ptr<ShaderComponent::ConstantBuffer>(new ShaderComponent::ConstantBuffer(&defaultPartCountBuffer, sizeof(ParticleCountBuffer)));

	// Set up sampler
	m_sampler = std::make_unique<ShaderComponent::Sampler>();


	// Compile shader and add to this shaderSet
	auto vsBlob = compileShader(L"ParticleShader.hlsl", "VSMain", "vs_5_0");
	setVertexShader(vsBlob);

	// Compile shader and add to this shaderSet
	auto gsBlob = compileShader(L"ParticleShader.hlsl", "GSMain", "gs_5_0");
	setGeometryShader(gsBlob);

	// Compile shader and add to this shaderSet
	auto psBlob = compileShader(L"ParticleShader.hlsl", "PSMain", "ps_5_0");
	setPixelShader(psBlob);

	// Compile shader
	auto csUpdateBlob = compileShader(L"ParticleCSUpdate.hlsl", "CSMain", "cs_5_0");

	// Compile shader
	auto csInsertBlob = compileShader(L"ParticleCSInsert.hlsl", "CSMain", "cs_5_0");

	// Set both the compute shaders
	ID3D10Blob** csBlobs = new ID3D10Blob*[2];
	csBlobs[0] = csInsertBlob;
	csBlobs[1] = csUpdateBlob;
	setComputeShaders(csBlobs, 2);

	// Done with the blobs, release them
	Memory::safeRelease(vsBlob);
	Memory::safeRelease(gsBlob);
	Memory::safeRelease(psBlob);
	Memory::safeRelease(csUpdateBlob);
	Memory::safeRelease(csInsertBlob);
	delete[] csBlobs;

	createBuffers();

}

ParticleShader::~ParticleShader() {
	Memory::safeRelease(m_currStateBuffer);
	Memory::safeRelease(m_nextStateBuffer);
	Memory::safeRelease(m_currSRV);
	Memory::safeRelease(m_currUAV);
	Memory::safeRelease(m_nextSRV);
	Memory::safeRelease(m_nextUAV);
	Memory::safeRelease(m_indirectArgsBuffer);
}

void ParticleShader::updateConstantBuffer() const {
	CameraDataBuffer data = { m_camUp, 0.f, m_camPos };
	m_cameraDataBuffer->updateData(&data, sizeof(data));
}

void ParticleShader::bind() {
	ShaderSet::bind();

	// No input layout is required since the input assembler is skipped
	Application::getInstance()->getDXManager()->getDeviceContext()->IASetInputLayout(nullptr);

	m_cameraDataBuffer->bind(ShaderComponent::GS, 0);
	m_modelDataBuffer->bind(ShaderComponent::GS, 1);
	m_sampler->bind(ShaderComponent::PS);
}

void ParticleShader::createBuffers() {
	// Create the vertex array that this shader uses
	ParticleShader::Vertex* vertices = new ParticleShader::Vertex[m_maxNumParticles];
	for (UINT i = 0; i < m_maxNumParticles; i++) {
		vertices[i].position = Vector3(0.f, 0.f, 0.f);
		vertices[i].direction = Vector3(0.f, 0.f, 0.f);
	}

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.ByteWidth = m_maxNumParticles * sizeof(Vertex);
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = sizeof(Vertex);

	D3D11_SUBRESOURCE_DATA initialDataCurr;
	ZeroMemory(&initialDataCurr, sizeof(initialDataCurr));
	initialDataCurr.pSysMem = vertices;
	initialDataCurr.SysMemPitch = 0;
	initialDataCurr.SysMemSlicePitch = 0;
	ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateBuffer(&bufferDesc, &initialDataCurr, &m_currStateBuffer));
	delete vertices;
	ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateBuffer(&bufferDesc, nullptr, &m_nextStateBuffer));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(uavDesc));
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = m_maxNumParticles;
	ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateUnorderedAccessView(m_currStateBuffer, &uavDesc, &m_currUAV));
	ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateUnorderedAccessView(m_nextStateBuffer, &uavDesc, &m_nextUAV));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = m_maxNumParticles;
	ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateShaderResourceView(m_currStateBuffer, &srvDesc, &m_currSRV));
	ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateShaderResourceView(m_nextStateBuffer, &srvDesc, &m_nextSRV));

	IndirectArgs IAs;
	IAs.vertexCount = 0;
	IAs.instanceCount = 1;
	IAs.startVertexLocation = 0;
	IAs.startInstanceLocation = 0;

	D3D11_SUBRESOURCE_DATA iaData;
	ZeroMemory(&iaData, sizeof(iaData));
	iaData.pSysMem = &IAs;
	iaData.SysMemPitch = 0;
	iaData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC IADesc;
	ZeroMemory(&IADesc, sizeof(IADesc));
	IADesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	IADesc.Usage = D3D11_USAGE_DEFAULT;
	IADesc.ByteWidth = sizeof(IndirectArgs);
	ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateBuffer(&IADesc, &iaData, &m_indirectArgsBuffer));



	//
	// Initial update run
	//

	// Binds the update CS
	bindCS(1);
	m_particleCountBuffer->bind(ShaderComponent::CS, 0);

	// -1 indicates to keep the current offset
	//UINT pUAVInitCounts = m_maxNumParticles;
	UINT pUAVInitCounts = -1;
	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(0, 1, &m_nextUAV, 0);
	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(1, 1, &m_currUAV, &pUAVInitCounts);
	Application::getInstance()->getDXManager()->getDeviceContext()->Dispatch(m_maxNumParticles / 512, 1, 1);
	ID3D11UnorderedAccessView* pNullUAV = nullptr;
	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(0, 1, &pNullUAV, nullptr);
	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(1, 1, &pNullUAV, nullptr);

	// Swap the UAVs
	std::swap(m_currUAV, m_nextUAV);

	// Update the constant buffer with the internal count of the current particle buffer
	Application::getInstance()->getDXManager()->getDeviceContext()->CopyStructureCount(m_particleCountBuffer->getBuffer(), 0, m_currUAV);
}

void ParticleShader::draw(bool bindFirst) {

	if (bindFirst) {
		// Bind the shaders
		// Bind the input layout
		// Bind constant buffer
		bind();
	}

	if (m_cameraDataHasChanged) {
		updateConstantBuffer();
		m_cameraDataHasChanged = false;
	}

	updateModelDataBuffer();

	ID3D11DeviceContext* devCon = Application::getInstance()->getDXManager()->getDeviceContext();

	devCon->PSSetShaderResources(0, 1, &m_texSRV);

	// Set topology
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	// Set the shader resource for the VS
	devCon->VSSetShaderResources(0, 1, &m_currSRV);

	devCon->CopyStructureCount(m_indirectArgsBuffer, 0, m_currUAV);
	devCon->DrawInstancedIndirect(m_indirectArgsBuffer, 0);

	// Unbind the shader resource
	ID3D11ShaderResourceView* nullSRV = nullptr;
	devCon->VSSetShaderResources(0, 1, &nullSRV);

	devCon->GSSetShader(nullptr, 0, 0);

}

void ParticleShader::updateCamera(Camera& cam, bool waterReflection) {
	m_vpMatrix = cam.getViewProjection();

	m_camPos = cam.getPosition();
	m_camUp = cam.getUp();
	m_cameraDataHasChanged = true;
}

void ParticleShader::updateModelDataBuffer() const {
	ModelDataBuffer data = {
		m_vpMatrix.Transpose()
	};

	m_modelDataBuffer->updateData(&data, sizeof(data));
}

void ParticleShader::updateParticleParameterBuffer(const DirectX::SimpleMath::Vector3& EmitterLocation, const DirectX::SimpleMath::Vector3& RandomVector) {
	ParticleParametersBuffer data = {
		EmitterLocation,
		0.f,
		RandomVector
	};

	m_particleParameterBuffer->updateData(&data, sizeof(data));
}

void ParticleShader::updateParticleCountBuffer(UINT numParticles) {
	ParticleCountBuffer data = { numParticles };

	m_particleCountBuffer->updateData(&data, sizeof(data));
}

void ParticleShader::setTextureSRV(ID3D11ShaderResourceView* srv) {
	m_texSRV = srv;
}

void ParticleShader::updateParticles(UINT& numParticles) {

	// Binds the update CS
	bindCS(1);
	m_particleCountBuffer->bind(ShaderComponent::BIND_SHADER::CS, 0);

	// -1 indicates to keep the current offset
	UINT pUAVInitCounts = -1;
	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(0, 1, &m_nextUAV, 0);
	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(1, 1, &m_currUAV, &pUAVInitCounts);
	UINT numGroups = (numParticles - numParticles % 512) / 512 + 1;
	Application::getInstance()->getDXManager()->getDeviceContext()->Dispatch(numGroups, 1, 1);
	ID3D11UnorderedAccessView* pNullUAV = nullptr;
	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(0, 1, &pNullUAV, nullptr);
	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(1, 1, &pNullUAV, nullptr);

	// Swap the UAVs
	std::swap(m_currUAV, m_nextUAV);

	Application::getInstance()->getDXManager()->getDeviceContext()->CopyStructureCount(m_particleCountBuffer->getBuffer(), 0, m_currUAV);
}

void ParticleShader::emitParticles(UINT& numBatches) {

	// Binds the insert CS
	bindCS(0);
	m_particleParameterBuffer->bind(ShaderComponent::CS, 0);

	// -1 indicates to keep the current offset
	UINT pUAVInitCounts = -1;
	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(0, 1, &m_currUAV, &pUAVInitCounts);
	Application::getInstance()->getDXManager()->getDeviceContext()->Dispatch(numBatches, 1, 1);
	ID3D11UnorderedAccessView* pNullUAV = nullptr;
	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(0, 1, &pNullUAV, nullptr);

	Application::getInstance()->getDXManager()->getDeviceContext()->CopyStructureCount(m_particleCountBuffer->getBuffer(), 0, m_currUAV);

}
