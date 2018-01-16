#pragma once

#include <Windows.h>
#include <d3d11.h>
#include "BindShader.h"

namespace ShaderComponent {

	class ConstantBuffer {

	public:
		ConstantBuffer(void* initData, UINT size);
		~ConstantBuffer();

		void updateData(void* newData, UINT bufferSize);

		void bind(BIND_SHADER bindShader, UINT slot = 0);

		ID3D11Buffer* getBuffer() {
			return m_buffer;
		}

	private:
		ID3D11Buffer* m_buffer;

	};

}