#include "Lights.h"

Lights::Lights()
	: m_numPls(0)
	, m_dlCam(4096.f, 2160.f, 0.1f, 1000.f)
{}
Lights::~Lights() {}

void Lights::addPointLight(const PointLight& pl) {
	m_pls.push_back(pl);
}
void Lights::setDirectionalLight(const DirectionalLight& dl) {
	m_dl = dl;
}

void Lights::setDirectionalLightCamera(const OrthographicCamera& dlCam) {
	m_dlCam = dlCam;
}

const Lights::DirectionalLight& Lights::getDL() const {
	return m_dl;
}
const std::vector<Lights::PointLight>& Lights::getPLs() const {
	return m_pls;
}

OrthographicCamera Lights::getDirectionalLightCamera() {
	return m_dlCam;
}