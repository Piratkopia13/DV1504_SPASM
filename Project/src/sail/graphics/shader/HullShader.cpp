#include "HullShader.h"

#include "../../Application.h"

HullShader::HullShader(ID3D10Blob* compiledShader) {
	Application::getInstance()->getDXManager()->getDevice()->CreateHullShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &m_shader);
}

HullShader::~HullShader() {
	Memory::safeRelease(m_shader);
}

void HullShader::bind() {
	Application::getInstance()->getDXManager()->getDeviceContext()->HSSetShader(m_shader, 0, 0);
}