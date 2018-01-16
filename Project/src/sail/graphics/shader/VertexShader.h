#pragma once

#include <d3d11.h>
#include <memory>
//#include "../../Application.h"

class VertexShader {
public:
	VertexShader(ID3D10Blob* compiledShader);
	~VertexShader();

	void bind();

private:
	ID3D11VertexShader* m_shader;

};