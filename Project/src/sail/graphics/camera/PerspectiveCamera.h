#pragma once

#include "Camera.h"

class PerspectiveCamera : public Camera {

public:
	PerspectiveCamera(float fov, float aspectRatio, float nearZ, float farZ) 
		: m_fov(fov)
		, m_nearZ(nearZ)
		, m_farZ(farZ)
	{
		m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_fov), aspectRatio, m_nearZ, m_farZ);
	};

	void resize(int width, int height) {
		m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_fov), static_cast<float>(width)/height, m_nearZ, m_farZ);
	}

private:
	virtual const DirectX::SimpleMath::Matrix& getProjectionMatrix() {
		return m_projectionMatrix;
	}

private:
	float m_fov;
	float m_nearZ;
	float m_farZ;

	DirectX::SimpleMath::Matrix m_projectionMatrix;

};
