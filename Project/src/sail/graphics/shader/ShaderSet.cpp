#include "ShaderSet.h"
#include "../../Application.h"

ShaderSet* ShaderSet::currentlyBoundShader = nullptr;

ShaderSet::ShaderSet()
	: m_vs(nullptr)
	, m_gs(nullptr)
	, m_ps(nullptr)
	, m_ds(nullptr)
	, m_hs(nullptr)
{

}
ShaderSet::~ShaderSet() {
}

void ShaderSet::bind() {

	// Don't bind if already bound
	// This is to cut down on shader state changes
	if (currentlyBoundShader == this)
		return;

	

	if (m_vs)
		m_vs->bind();
	else
		Application::getInstance()->getDXManager()->getDeviceContext()->VSSetShader(nullptr, 0, 0);
	if (m_gs)
		m_gs->bind();
	else
		Application::getInstance()->getDXManager()->getDeviceContext()->GSSetShader(nullptr, 0, 0);
	if (m_ps)
		m_ps->bind();
	else
		Application::getInstance()->getDXManager()->getDeviceContext()->PSSetShader(nullptr, 0, 0);
	if (m_ds)
		m_ds->bind();
	else
		Application::getInstance()->getDXManager()->getDeviceContext()->DSSetShader(nullptr, 0, 0);
	if (m_hs)
		m_hs->bind();
	else
		Application::getInstance()->getDXManager()->getDeviceContext()->HSSetShader(nullptr, 0, 0);

	// Set this shader as bound
	//currentlyBoundShader = this;

}

void ShaderSet::bindCS(UINT csIndex) {
	if (m_css[csIndex]) 
		m_css[csIndex]->bind();
	
}

ID3D10Blob* ShaderSet::compileShader(LPCWSTR filename, LPCSTR entryPoint, LPCSTR shaderVersion) {
	
	ID3D10Blob *shader = nullptr;
	ID3D10Blob *errorMsg;
	if (FAILED(D3DCompileFromFile((DEFAULT_SHADER_LOCATION + filename).c_str(), 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderVersion, 0, 0, &shader, &errorMsg))) {
		OutputDebugString(L"\n Failed to compile shader\n\n");

		char* msg = (char*)(errorMsg->GetBufferPointer());

		for (size_t i = 0; i < errorMsg->GetBufferSize(); i++) {
			std::cout << msg[i];
		}
	}

	return shader;

}

void ShaderSet::setVertexShader(ID3D10Blob* blob) {

	m_vs = std::make_unique<VertexShader>(blob);

}
void ShaderSet::setGeometryShader(ID3D10Blob* blob) {

	m_gs = std::make_unique<GeometryShader>(blob);

}
void ShaderSet::setPixelShader(ID3D10Blob* blob) {

	m_ps = std::make_unique<PixelShader>(blob);

}
void ShaderSet::setComputeShaders(ID3D10Blob** blob, UINT numBlobs) {
	m_css.resize(numBlobs);

	for (UINT i = 0; i < numBlobs; i++) {
		m_css[i] = std::make_unique<ComputeShader>(blob[i]);
	}
}
void ShaderSet::setDomainShader(ID3D10Blob* blob) {

	m_ds = std::make_unique<DomainShader>(blob);

}
void ShaderSet::setHullShader(ID3D10Blob* blob) {

	m_hs = std::make_unique<HullShader>(blob);

}
//VertexShader* ShaderSet::getVertexShader() {
//	return m_vs.get();
//}