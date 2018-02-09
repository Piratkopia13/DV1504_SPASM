#include "Sampler.h"
#include "../../../Application.h"

namespace ShaderComponent {

	Sampler::Sampler(D3D11_TEXTURE_ADDRESS_MODE addressMode, D3D11_FILTER filter) {

		// Set up sampler
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.AddressU = addressMode;
		desc.AddressV = addressMode;
		desc.AddressW = addressMode;
		desc.Filter = filter;
		desc.MipLODBias = 0.f;
		desc.MaxAnisotropy = 0;
		desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;

		Application::getInstance()->getDXManager()->getDevice()->CreateSamplerState(&desc, &m_samplerState);

	}

	Sampler::~Sampler() {
		Memory::safeRelease(m_samplerState);
	}

	void Sampler::bind(BIND_SHADER bindShader, UINT slot) {
		switch (bindShader) {
		case ShaderComponent::VS:
			Application::getInstance()->getDXManager()->getDeviceContext()->VSSetSamplers(slot, 1, &m_samplerState);
			break;
		case ShaderComponent::HS:
			Application::getInstance()->getDXManager()->getDeviceContext()->HSSetSamplers(slot, 1, &m_samplerState);
			break;
		case ShaderComponent::DS:
			Application::getInstance()->getDXManager()->getDeviceContext()->DSSetSamplers(slot, 1, &m_samplerState);
			break;
		case ShaderComponent::PS:
			Application::getInstance()->getDXManager()->getDeviceContext()->PSSetSamplers(slot, 1, &m_samplerState);
			break;
		case ShaderComponent::GS:
			Application::getInstance()->getDXManager()->getDeviceContext()->GSSetSamplers(slot, 1, &m_samplerState);
			break;
		case ShaderComponent::CS:
			Application::getInstance()->getDXManager()->getDeviceContext()->CSSetSamplers(slot, 1, &m_samplerState);
			break;
		default:
			break;
		}
	}

}