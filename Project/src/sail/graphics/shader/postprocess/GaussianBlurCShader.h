#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include "../component/ConstantBuffer.h"
#include "../component/Sampler.h"
#include "../../camera/Camera.h"
#include "../../../Application.h"
#include "../ShaderSet.h"
#include "../../RenderableTexture.h"

class GaussianBlurCShader : public ShaderSet {
public:
	GaussianBlurCShader();
	~GaussianBlurCShader();

	void bind() override;

	void createBufferFromModelData(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer, const void* data) {}

	void createBuffers();

	virtual void draw(bool bindFirst = true);

	void setOutputTexture(ID3D11Texture2D* tex);
 	void setInputSRV(ID3D11ShaderResourceView** srv);

	void resize(int width, int height);

private:
	void updateConstantBuffer() const;
	void setHorPassUAV(ID3D11Texture2D* tex);

private:

	ID3D11UnorderedAccessView* m_horPassUAV;
	ID3D11UnorderedAccessView* m_vertPassUAV;

	ID3D11ShaderResourceView** m_horInputSRV;
	ID3D11ShaderResourceView** m_vertInputSRV;

	std::unique_ptr<RenderableTexture> m_middleTex;

	// Components

};