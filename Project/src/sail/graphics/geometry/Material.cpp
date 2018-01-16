#include "Material.h"

using namespace DirectX;
using namespace SimpleMath;

Material::Material()
	: m_ka(1.f)
	, m_kd(1.f)
	, m_ks(1.f)
	, m_shininess(10.f)
	, m_color(Vector4::One)
	, m_customSRVs(nullptr)
	, m_numTextures(3)
{
	m_textureFlags[0] = false;
	m_textureFlags[1] = false;
	m_textureFlags[2] = false;
}
Material::~Material() { }

void Material::setKa(float ka) {
	m_ka = ka;
}
void Material::setKd(float kd) {
	m_kd = kd;
}
void Material::setKs(float ks) {
	m_ks = ks;
}
void Material::setShininess(float shininess) {
	m_shininess = shininess;
}
void Material::setColor(const DirectX::SimpleMath::Vector4& color) {
	m_color = color;
}


void Material::setDiffuseTexture(const std::string& filename) {
	getAndInsertTexture(filename, 0);
	m_textureFlags[0] = true;
}
void Material::setDiffuseTexture(ID3D11ShaderResourceView* srv) {
	m_srvs[0] = srv;
	m_textureFlags[0] = true;
}


void Material::setNormalTexture(const std::string& filename) {
	getAndInsertTexture(filename, 1);
	m_textureFlags[1] = true;
}
void Material::setNormalTexture(ID3D11ShaderResourceView* srv) {
	m_srvs[1] = srv;
	m_textureFlags[1] = true;
}


void Material::setSpecularTexture(const std::string& filename) {
	getAndInsertTexture(filename, 2);
	m_textureFlags[2] = true;
}
void Material::setSpecularTexture(ID3D11ShaderResourceView* srv) {
	m_srvs[2] = srv;
	m_textureFlags[2] = true;
}


void Material::setTextures(ID3D11ShaderResourceView** srvs, UINT numTextures) {
	m_numTextures = numTextures;
	m_customSRVs = srvs;
}


void Material::getAndInsertTexture(const std::string& filename, int arraySlot) {

	DXTexture* t = &Application::getInstance()->getResourceManager().getDXTexture(filename);
	m_srvs[arraySlot] = *t->getResourceView();

}


ID3D11ShaderResourceView* const* Material::getTextures(UINT& numTextures) {
	numTextures = m_numTextures;
	if (m_customSRVs)
		return m_customSRVs;
	else
		return m_srvs;
}

float Material::getKa() const {
	return m_ka;
}
float Material::getKd() const {
	return m_kd;
}
float Material::getKs() const {
	return m_ks;
}
float Material::getShininess() const {
	return m_shininess;
}
const DirectX::SimpleMath::Vector4& Material::getColor() const {
	return m_color;
}
const bool* Material::getTextureFlags() const {
	return m_textureFlags;
}