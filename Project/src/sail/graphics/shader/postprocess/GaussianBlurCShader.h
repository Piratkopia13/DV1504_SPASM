#pragma once

#include <d3d11.h>
#include <SimpleMath.h>

#include "../component/ConstantBuffer.h"
#include "../component/Sampler.h"
#include "../../camera/Camera.h"
#include "../../../Application.h"
#include "../ShaderSet.h"

class GaussianBlurCShader : public ShaderSet {
public:
	GaussianBlurCShader();
	~GaussianBlurCShader();

	void bind() override;

	void createBufferFromModelData(ID3D11Buffer** vertexBuffer, ID3D11Buffer** indexBuffer, const void* data) {}

	void createBuffers();

	virtual void draw(bool bindFirst = true);

	void setInputSRV(ID3D11ShaderResourceView** srv);
	void setInputTexture(ID3D11Texture2D* tex);
	void setOutputTexture(ID3D11Texture2D* tex);
	void setOutputSRV(ID3D11ShaderResourceView** srv);

	//void updateModelDataBuffer() const;

	//void setTextureSRV(ID3D11ShaderResourceView* srv);

	//void updateParticles(UINT& numParticles);
	//void emitParticles(UINT& numBatches);

private:
	void updateConstantBuffer() const;

private:

	ID3D11UnorderedAccessView* m_horPassUAV;
	ID3D11UnorderedAccessView* m_vertPassUAV;

	ID3D11ShaderResourceView** m_horInputSRV;
	ID3D11ShaderResourceView** m_vertInputSRV;

	// Components

};