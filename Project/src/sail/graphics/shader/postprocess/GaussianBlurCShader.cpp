#include "GaussianBlurCShader.h"


using namespace DirectX::SimpleMath;

GaussianBlurCShader::GaussianBlurCShader()
	: m_horInputSRV(nullptr)
	, m_vertInputSRV(nullptr)
	, m_horPassUAV(nullptr)
	, m_vertPassUAV(nullptr)
{

	// Set up sampler
	//m_sampler = std::make_unique<ShaderComponent::Sampler>();


	// Compile shader
	auto csHorBlob = compileShader(L"postprocess/GaussianBlurCS.hlsl", "CSHorizontal", "cs_5_0");
	auto csVertBlob = compileShader(L"postprocess/GaussianBlurCS.hlsl", "CSVertical", "cs_5_0");


	// Set both the compute shaders
	ID3D10Blob** csBlobs = new ID3D10Blob*[2];
	csBlobs[0] = csHorBlob;
	csBlobs[1] = csVertBlob;
	setComputeShaders(csBlobs, 2);

	// Done with the blobs, release them
	Memory::safeRelease(csHorBlob);
	Memory::safeRelease(csVertBlob);
	delete[] csBlobs;

	createBuffers();

}

GaussianBlurCShader::~GaussianBlurCShader() {
	Memory::safeRelease(m_horPassUAV);
	Memory::safeRelease(m_vertPassUAV);

	/*Memory::safeRelease(m_currStateBuffer);
	Memory::safeRelease(m_nextStateBuffer);
	Memory::safeRelease(m_currSRV);
	Memory::safeRelease(m_currUAV);
	Memory::safeRelease(m_nextSRV);
	Memory::safeRelease(m_nextUAV);
	Memory::safeRelease(m_indirectArgsBuffer);*/
}

void GaussianBlurCShader::updateConstantBuffer() const {
	/*CameraDataBuffer data = { m_camUp, 0.f, m_camPos };
	m_cameraDataBuffer->updateData(&data, sizeof(data));*/
}

void GaussianBlurCShader::bind() {
	//ShaderSet::bind();

	// No input layout is required since the input assembler is skipped
	//Application::getInstance()->getDXManager()->getDeviceContext()->IASetInputLayout(nullptr);
/*
	m_cameraDataBuffer->bind(ShaderComponent::GS, 0);
	m_modelDataBuffer->bind(ShaderComponent::GS, 1);
	m_sampler->bind(ShaderComponent::PS);*/
}

void GaussianBlurCShader::createBuffers() {
	//// Create the vertex array that this shader uses
	//GaussianBlurCShader::Vertex* vertices = new GaussianBlurCShader::Vertex[m_maxNumParticles];
	//for (UINT i = 0; i < m_maxNumParticles; i++) {
	//	vertices[i].position = Vector3(0.f, 0.f, 0.f);
	//	vertices[i].direction = Vector3(0.f, 0.f, 0.f);
	//}

	//D3D11_BUFFER_DESC bufferDesc;
	//ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	//bufferDesc.ByteWidth = m_maxNumParticles * sizeof(Vertex);
	//bufferDesc.CPUAccessFlags = 0;
	//bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	//bufferDesc.StructureByteStride = sizeof(Vertex);

	//D3D11_SUBRESOURCE_DATA initialDataCurr;
	//ZeroMemory(&initialDataCurr, sizeof(initialDataCurr));
	//initialDataCurr.pSysMem = vertices;
	//initialDataCurr.SysMemPitch = 0;
	//initialDataCurr.SysMemSlicePitch = 0;
	//ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateBuffer(&bufferDesc, &initialDataCurr, &m_currStateBuffer));
	//delete vertices;
	//ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateBuffer(&bufferDesc, nullptr, &m_nextStateBuffer));

	//D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	//ZeroMemory(&uavDesc, sizeof(uavDesc));
	//uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	//uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	//uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	//uavDesc.Buffer.FirstElement = 0;
	//uavDesc.Buffer.NumElements = m_maxNumParticles;
	//ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateUnorderedAccessView(m_currStateBuffer, &uavDesc, &m_currUAV));
	//ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateUnorderedAccessView(m_nextStateBuffer, &uavDesc, &m_nextUAV));

	//D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	//ZeroMemory(&srvDesc, sizeof(srvDesc));
	//srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	//srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	//srvDesc.Buffer.FirstElement = 0;
	//srvDesc.Buffer.NumElements = m_maxNumParticles;
	//ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateShaderResourceView(m_currStateBuffer, &srvDesc, &m_currSRV));
	//ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateShaderResourceView(m_nextStateBuffer, &srvDesc, &m_nextSRV));

	//IndirectArgs IAs;
	//IAs.vertexCount = 0;
	//IAs.instanceCount = 1;
	//IAs.startVertexLocation = 0;
	//IAs.startInstanceLocation = 0;

	//D3D11_SUBRESOURCE_DATA iaData;
	//ZeroMemory(&iaData, sizeof(iaData));
	//iaData.pSysMem = &IAs;
	//iaData.SysMemPitch = 0;
	//iaData.SysMemSlicePitch = 0;

	//D3D11_BUFFER_DESC IADesc;
	//ZeroMemory(&IADesc, sizeof(IADesc));
	//IADesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	//IADesc.Usage = D3D11_USAGE_DEFAULT;
	//IADesc.ByteWidth = sizeof(IndirectArgs);
	//ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateBuffer(&IADesc, &iaData, &m_indirectArgsBuffer));



	////
	//// Initial update run
	////

	//// Binds the update CS
	//bindCS(1);
	//m_particleCountBuffer->bind(ShaderComponent::CS, 0);

	//// -1 indicates to keep the current offset
	////UINT pUAVInitCounts = m_maxNumParticles;
	//UINT pUAVInitCounts = -1;
	//Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(0, 1, &m_nextUAV, 0);
	//Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(1, 1, &m_currUAV, &pUAVInitCounts);
	//Application::getInstance()->getDXManager()->getDeviceContext()->Dispatch(m_maxNumParticles / 512, 1, 1);
	//ID3D11UnorderedAccessView* pNullUAV = nullptr;
	//Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(0, 1, &pNullUAV, nullptr);
	//Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(1, 1, &pNullUAV, nullptr);

	//// Swap the UAVs
	//std::swap(m_currUAV, m_nextUAV);

	//// Update the constant buffer with the internal count of the current particle buffer
	//Application::getInstance()->getDXManager()->getDeviceContext()->CopyStructureCount(m_particleCountBuffer->getBuffer(), 0, m_currUAV);
}

void GaussianBlurCShader::setInputSRV(ID3D11ShaderResourceView** srv) {
	m_horInputSRV = srv;
}
void GaussianBlurCShader::setOutputTexture(ID3D11Texture2D* tex) {
	auto* dev = Application::getInstance()->getDXManager()->getDevice();
	D3D11_TEXTURE2D_DESC texDesc;
	tex->GetDesc(&texDesc);

	Memory::safeRelease(m_horPassUAV);

	D3D11_UNORDERED_ACCESS_VIEW_DESC descView;
	ZeroMemory(&descView, sizeof(descView));
	descView.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	descView.Buffer.FirstElement = 0;
	descView.Buffer.NumElements = texDesc.Width * texDesc.Height;
	descView.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	ThrowIfFailed(dev->CreateUnorderedAccessView(tex, &descView, &m_horPassUAV));
}

void GaussianBlurCShader::setInputTexture(ID3D11Texture2D* tex) {
	auto* dev = Application::getInstance()->getDXManager()->getDevice();
	D3D11_TEXTURE2D_DESC texDesc;
	tex->GetDesc(&texDesc);

	Memory::safeRelease(m_vertPassUAV);

	D3D11_UNORDERED_ACCESS_VIEW_DESC descView;
	ZeroMemory(&descView, sizeof(descView));
	descView.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	descView.Buffer.FirstElement = 0;
	descView.Buffer.NumElements = texDesc.Width * texDesc.Height;
	descView.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

	ThrowIfFailed(dev->CreateUnorderedAccessView(tex, &descView, &m_vertPassUAV));
}

void GaussianBlurCShader::setOutputSRV(ID3D11ShaderResourceView** srv) {
	m_vertInputSRV = srv;
}


void GaussianBlurCShader::draw(bool bindFirst) {

	Application* app = Application::getInstance();
	ID3D11DeviceContext* con = app->getDXManager()->getDeviceContext();
	UINT groupsX = app->getWindow()->getWindowWidth();
	UINT groupsY = app->getWindow()->getWindowHeight();
	UINT initCounts = 1;

	ID3D11UnorderedAccessView* pNullUAV = nullptr;
	ID3D11ShaderResourceView* pNullSRV = nullptr;

	//////////////////////////////////////////////////////////////////////////
	//							DO HORIZONTAL BLUR PASS
	//////////////////////////////////////////////////////////////////////////
	bindCS(0);

	con->CSSetUnorderedAccessViews(0, 1, &m_horPassUAV, nullptr);
	con->CSSetShaderResources(0, 1, m_horInputSRV);
	con->Dispatch(groupsX, groupsY, 1);

	// Unbind resources from shader
	con->CSSetUnorderedAccessViews(0, 1, &pNullUAV, nullptr);
	con->CSSetShaderResources(0, 1, &pNullSRV);

	//////////////////////////////////////////////////////////////////////////
	//							DO VERTICAL BLUR PASS
	//////////////////////////////////////////////////////////////////////////
	bindCS(1);

	con->CSSetUnorderedAccessViews(0, 1, &m_vertPassUAV, nullptr);
	con->CSSetShaderResources(0, 1, m_vertInputSRV);
	con->Dispatch(groupsX, groupsY, 1);


	// Unbind resources from shader
	con->CSSetUnorderedAccessViews(0, 1, &pNullUAV, nullptr);
	con->CSSetShaderResources(0, 1, &pNullSRV);

}

//void GaussianBlurCShader::updateParticles(UINT& numParticles) {
//
//	// Binds the update CS
//	bindCS(1);
//	m_particleCountBuffer->bind(ShaderComponent::BIND_SHADER::CS, 0);
//
//	// -1 indicates to keep the current offset
//	UINT pUAVInitCounts = -1;
//	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(0, 1, &m_nextUAV, 0);
//	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(1, 1, &m_currUAV, &pUAVInitCounts);
//	UINT numGroups = (numParticles - numParticles % 512) / 512 + 1;
//	Application::getInstance()->getDXManager()->getDeviceContext()->Dispatch(numGroups, 1, 1);
//	ID3D11UnorderedAccessView* pNullUAV = nullptr;
//	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(0, 1, &pNullUAV, nullptr);
//	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(1, 1, &pNullUAV, nullptr);
//
//	// Swap the UAVs
//	std::swap(m_currUAV, m_nextUAV);
//
//	Application::getInstance()->getDXManager()->getDeviceContext()->CopyStructureCount(m_particleCountBuffer->getBuffer(), 0, m_currUAV);
//}

//void GaussianBlurCShader::emitParticles(UINT& numBatches) {
//
//	// Binds the insert CS
//	bindCS(0);
//	m_particleParameterBuffer->bind(ShaderComponent::CS, 0);
//
//	// -1 indicates to keep the current offset
//	UINT pUAVInitCounts = -1;
//	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(0, 1, &m_currUAV, &pUAVInitCounts);
//	Application::getInstance()->getDXManager()->getDeviceContext()->Dispatch(numBatches, 1, 1);
//	ID3D11UnorderedAccessView* pNullUAV = nullptr;
//	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetUnorderedAccessViews(0, 1, &pNullUAV, nullptr);
//
//	Application::getInstance()->getDXManager()->getDeviceContext()->CopyStructureCount(m_particleCountBuffer->getBuffer(), 0, m_currUAV);
//
//}