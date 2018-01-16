#include "VertexShader.h"
#include "../../Application.h"

VertexShader::VertexShader(ID3D10Blob* compiledShader) {
	Application::getInstance()->getDXManager()->getDevice()->CreateVertexShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &m_shader);
}

VertexShader::~VertexShader() {
	Memory::safeRelease(m_shader);
}

void VertexShader::bind() {
	Application::getInstance()->getDXManager()->getDeviceContext()->VSSetShader(m_shader, 0, 0);
}