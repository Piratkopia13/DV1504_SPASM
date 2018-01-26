#include "GaussianBlurCShader.h"


using namespace DirectX::SimpleMath;

GaussianBlurCShader::GaussianBlurCShader()
	: m_horInputSRV(nullptr)
	, m_vertInputSRV(nullptr)
	, m_horPassUAV(nullptr)
	, m_vertPassUAV(nullptr)
{

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
}

void GaussianBlurCShader::updateConstantBuffer() const {

}

void GaussianBlurCShader::bind() {

}

void GaussianBlurCShader::createBuffers() {

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
 	UINT windowWidth = app->getWindow()->getWindowWidth();
	UINT windowHeight = app->getWindow()->getWindowHeight();
	UINT initCounts = 1;

	ID3D11UnorderedAccessView* pNullUAV = nullptr;
	ID3D11ShaderResourceView* pNullSRV = nullptr;

	//////////////////////////////////////////////////////////////////////////
	//							DO HORIZONTAL BLUR PASS
	//////////////////////////////////////////////////////////////////////////
	bindCS(0);

	con->CSSetUnorderedAccessViews(0, 1, &m_horPassUAV, nullptr);
	con->CSSetShaderResources(0, 1, m_horInputSRV);

	// Do as many "full" dispatches as possible
	UINT32 numFullPases = static_cast<UINT32>(ceilf(windowWidth / 1024.0f));
	con->Dispatch(1, windowHeight, numFullPases);


	//////////////////////////////////////////////////////////////////////////
	//							DO VERTICAL BLUR PASS
	//////////////////////////////////////////////////////////////////////////
	bindCS(1);

	con->CSSetUnorderedAccessViews(0, 1, &m_vertPassUAV, nullptr);
	con->CSSetShaderResources(0, 1, m_vertInputSRV);

	// Do as many "full" dispatches as possible
	con->Dispatch(windowWidth, 1, numFullPases);
	

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