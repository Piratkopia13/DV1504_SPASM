#include "GaussianBlurCShader.h"


using namespace DirectX::SimpleMath;

GaussianBlurCShader::GaussianBlurCShader()
	: m_horInputSRV(nullptr)
	, m_vertInputSRV(nullptr)
	, m_horPassUAV(nullptr)
	, m_vertPassUAV(nullptr)
{

	auto window = Application::getInstance()->getWindow();
	UINT width = window->getWindowWidth();
	UINT height = window->getWindowHeight();

	// Set up the "middle" texture used betwwen the two passes
	m_middleTex = std::unique_ptr<RenderableTexture>(new RenderableTexture(1U, width, height, false, false, D3D11_BIND_UNORDERED_ACCESS));
	m_vertInputSRV = m_middleTex->getColorSRV();
	setHorPassUAV(m_middleTex->getTexture2D());


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

void GaussianBlurCShader::setHorPassUAV(ID3D11Texture2D* tex) {
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

void GaussianBlurCShader::setOutputTexture(ID3D11Texture2D* tex) {
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

void GaussianBlurCShader::resize(int width, int height) {
	m_middleTex->resize(width, height);
	m_vertInputSRV = m_middleTex->getColorSRV();
	setHorPassUAV(m_middleTex->getTexture2D());
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
