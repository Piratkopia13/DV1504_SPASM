#include "GeometryShader.h"

#include "../../Application.h"

GeometryShader::GeometryShader(ID3D10Blob* compiledShader) {
	Application::getInstance()->getDXManager()->getDevice()->CreateGeometryShader(compiledShader->GetBufferPointer(), compiledShader->GetBufferSize(), NULL, &m_shader);
}

GeometryShader::~GeometryShader() {
	Memory::safeRelease(m_shader);
}

void GeometryShader::bind() {
	Application::getInstance()->getDXManager()->getDeviceContext()->GSSetShader(m_shader, 0, 0);
}