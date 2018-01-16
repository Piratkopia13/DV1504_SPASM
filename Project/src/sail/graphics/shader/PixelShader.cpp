#include "PixelShader.h"
#include "../../Application.h"

PixelShader::PixelShader(ID3D10Blob* compiledShader) {
	Application::getInstance()->getDXManager()->getDevice()->CreatePixelShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &m_shader);
}

PixelShader::~PixelShader() {
	Memory::safeRelease(m_shader);
}

void PixelShader::bind() {
	Application::getInstance()->getDXManager()->getDeviceContext()->PSSetShader(m_shader, 0, 0);
}