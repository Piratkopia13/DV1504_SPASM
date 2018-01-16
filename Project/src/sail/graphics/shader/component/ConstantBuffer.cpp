#include "ConstantBuffer.h"
#include "../../../Application.h"

namespace ShaderComponent {

	ConstantBuffer::ConstantBuffer(void* initData, UINT size) {

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.ByteWidth = size;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = initData;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		ThrowIfFailed(Application::getInstance()->getDXManager()->getDevice()->CreateBuffer(&desc, &data, &m_buffer));

	}
	ConstantBuffer::~ConstantBuffer() {
		Memory::safeRelease(m_buffer);
	}

	void ConstantBuffer::updateData(void* newData, UINT bufferSize) {

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		Application::getInstance()->getDXManager()->getDeviceContext()->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		std::memcpy(mappedResource.pData, newData, bufferSize);
		Application::getInstance()->getDXManager()->getDeviceContext()->Unmap(m_buffer, 0);

	}

	void ConstantBuffer::bind(BIND_SHADER bindShader, UINT slot) {
		switch (bindShader) {
		case ShaderComponent::VS:
			Application::getInstance()->getDXManager()->getDeviceContext()->VSSetConstantBuffers(slot, 1, &m_buffer);
			break;
		case ShaderComponent::HS:
			Application::getInstance()->getDXManager()->getDeviceContext()->HSSetConstantBuffers(slot, 1, &m_buffer);
			break;
		case ShaderComponent::DS:
			Application::getInstance()->getDXManager()->getDeviceContext()->DSSetConstantBuffers(slot, 1, &m_buffer);
			break;
		case ShaderComponent::PS:
			Application::getInstance()->getDXManager()->getDeviceContext()->PSSetConstantBuffers(slot, 1, &m_buffer);
			break;
		case ShaderComponent::GS:
			Application::getInstance()->getDXManager()->getDeviceContext()->GSSetConstantBuffers(slot, 1, &m_buffer);
			break;
		case ShaderComponent::CS:
			Application::getInstance()->getDXManager()->getDeviceContext()->CSSetConstantBuffers(slot, 1, &m_buffer);
			break;
		default:
			break;
		}
	}

}