#pragma once

#include <string>
#include <d3d11.h>
#include <SimpleMath.h>
#include "../../resources/DXTexture.h"
#include "../../Application.h"

class Material {

public:
	Material();
	~Material();

	void setKa(float ka);
	void setKd(float kd);
	void setKs(float ks);
	void setShininess(float shininess);
	void setColor(const DirectX::SimpleMath::Vector4& color);

	void setDiffuseTexture(const std::string& filename);
	void setDiffuseTexture(ID3D11ShaderResourceView* srv);

	void setNormalTexture(const std::string& filename);
	void setNormalTexture(ID3D11ShaderResourceView* srv);

	void setSpecularTexture(const std::string& filename);
	void setSpecularTexture(ID3D11ShaderResourceView* srv);

	void setTextures(ID3D11ShaderResourceView** srvs, UINT numTextures);

	/*	Returns an array of textures, numTextures get set
		Default texture order in array is as follows
		0 - Diffuse texture
		1 - Normal map
		2 - Specular map
	*/ 
	ID3D11ShaderResourceView* const* getTextures(UINT& numTextures);

	float getKa() const;
	float getKd() const;
	float getKs() const;
	float getShininess() const;
	const DirectX::SimpleMath::Vector4& getColor() const;
	const bool* getTextureFlags() const;

private:
	void getAndInsertTexture(const std::string& filename, int arraySlot);

private:
	float m_ka;
	float m_kd;
	float m_ks;
	float m_shininess;
	DirectX::SimpleMath::Vector4 m_color;

	bool m_textureFlags[3];
	ID3D11ShaderResourceView* m_srvs[3];
	UINT m_numTextures;

	ID3D11ShaderResourceView** m_customSRVs;

};