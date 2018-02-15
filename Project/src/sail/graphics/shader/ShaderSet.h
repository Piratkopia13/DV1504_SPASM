#pragma once

#include <memory>
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <iostream>
#include <SimpleMath.h>

#include "VertexShader.h"
#include "GeometryShader.h"
#include "PixelShader.h"
#include "ComputeShader.h"
#include "DomainShader.h"
#include "HullShader.h"
#include "../geometry/Model.h"
#include "../camera/Camera.h"

namespace {
	static const std::wstring DEFAULT_SHADER_LOCATION = L"res/shaders/";
}

class ShaderSet {

	friend class Text;

public:
	ShaderSet();
	virtual ~ShaderSet();

	virtual void bind();
	virtual void bindCS(UINT csIndex = 0);

	virtual void draw(Model& model, bool bindFirst = true) {}
	virtual void draw(bool bindFirst = true) {}

	virtual void createBufferFromModelData(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer, ID3D11Buffer** instanceBuffer, const void* data) = 0;

	virtual void updateCamera(Camera& cam) {};
	virtual void setClippingPlane(const DirectX::SimpleMath::Vector4& clippingPlane) {};

	static ID3D10Blob* compileShader(LPCWSTR filename, LPCSTR entryPoint, LPCSTR shaderVersion);
protected:

	void setVertexShader(ID3D10Blob* blob);
	void setGeometryShader(ID3D10Blob* blob);
	void setPixelShader(ID3D10Blob* blob);
	void setComputeShaders(ID3D10Blob** blob, UINT numBlobs);
	void setDomainShader(ID3D10Blob* blob);
	void setHullShader(ID3D10Blob* blob);

protected:
	static ShaderSet* currentlyBoundShader;

private:
	std::unique_ptr<VertexShader> m_vs;
	std::unique_ptr<GeometryShader> m_gs;
	std::unique_ptr<PixelShader> m_ps;
	std::unique_ptr<DomainShader> m_ds;
	std::unique_ptr<HullShader> m_hs;
	std::vector<std::unique_ptr<ComputeShader>> m_css;

};