#pragma once

#include <d3d11.h>
#include <SimpleMath.h>
#include <vector>
#include "camera/OrthographicCamera.h"

class Lights {
public:

	struct PointLightStruct {
		DirectX::SimpleMath::Vector3 color;
		float padding;
		DirectX::SimpleMath::Vector3 position;
		float attenuation;
	};

	class PointLight {
	public:
		struct Attenuation {
			float constant;
			float linear;
			float quadratic;
		};
	public:
		PointLight() : m_color(DirectX::SimpleMath::Vector3::Zero), m_position(DirectX::SimpleMath::Vector3::Zero), m_attenuation({1.f, 1.f, 1.f}) {}
		void setColor(const DirectX::SimpleMath::Vector3& color) { m_color = color; }
		const DirectX::SimpleMath::Vector3& getColor() const { return m_color; }
		void setPosition(const DirectX::SimpleMath::Vector3& position) { m_position = position; }
		const DirectX::SimpleMath::Vector3& getPosition() const { return m_position; }
		void setAttenuation(float constant, float linear, float quadratic) { 
			m_attenuation.constant = constant;
			m_attenuation.linear = linear;
			m_attenuation.quadratic = quadratic;
			calculateRadius();
		}
		const Attenuation& getAttenuation() const { return m_attenuation; }
		float getRadius() const { return m_radius; }
	private:
		void calculateRadius() {
			// Derived from attenuation formula used in light shader
			m_radius = (-m_attenuation.linear + std::sqrt(std::pow(m_attenuation.linear, 2.f) - 4.f * m_attenuation.quadratic*(m_attenuation.constant-100))) / (2*m_attenuation.quadratic);
		}
	private:
		DirectX::SimpleMath::Vector3 m_color;
		DirectX::SimpleMath::Vector3 m_position;
		Attenuation m_attenuation;
		float m_radius;
	};

	struct DirectionalLight {
		DirectionalLight() : color(DirectX::SimpleMath::Vector3(.8f)), direction(DirectX::SimpleMath::Vector3(0.f, -1.f, 0.f)) {}
		DirectX::SimpleMath::Vector3 color;
		float padding1;
		DirectX::SimpleMath::Vector3 direction;
		float padding2;
	};

public:
	Lights();
	~Lights();

	void addPointLight(const PointLight& pl);
	void setDirectionalLight(const DirectionalLight& dl);
	void setDirectionalLightCamera(const OrthographicCamera& dlCam);

	const DirectionalLight& getDL() const;
	const std::vector<PointLight>& getPLs() const;
	OrthographicCamera getDirectionalLightCamera();



private:

	DirectionalLight m_dl;
	std::vector<PointLight> m_pls;
	int m_numPls;

	OrthographicCamera m_dlCam;

};