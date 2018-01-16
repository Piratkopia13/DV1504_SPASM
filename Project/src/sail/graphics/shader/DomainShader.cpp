#include "DomainShader.h"

#include "../../Application.h"

DomainShader::DomainShader(ID3D10Blob* compiledShader) {
	Application::getInstance()->getDXManager()->getDevice()->CreateDomainShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &m_shader);
}

DomainShader::~DomainShader() {
	Memory::safeRelease(m_shader);
}

void DomainShader::bind() {
	Application::getInstance()->getDXManager()->getDeviceContext()->DSSetShader(m_shader, 0, 0);
}