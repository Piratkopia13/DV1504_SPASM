#include "ComputeShader.h"

#include "../../Application.h"

ComputeShader::ComputeShader(ID3D10Blob* compiledShader) {
	Application::getInstance()->getDXManager()->getDevice()->CreateComputeShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &m_shader);
}

ComputeShader::~ComputeShader() {
	Memory::safeRelease(m_shader);
}

void ComputeShader::bind() {
	Application::getInstance()->getDXManager()->getDeviceContext()->CSSetShader(m_shader, 0, 0);
}