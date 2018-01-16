#pragma once

#include <Windows.h>
#include <d3d11.h>
#include "BindShader.h"

namespace ShaderComponent {

	class Sampler {

	public:
		Sampler(D3D11_TEXTURE_ADDRESS_MODE adressMode = D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR);
		~Sampler();

		void bind(BIND_SHADER bindShader = PS, UINT slot = 0);

	private:
		ID3D11SamplerState* m_samplerState;


	};

}