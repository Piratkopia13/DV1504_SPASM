#pragma once

#include <map>
#include <vector>
#include "shader/ShaderSet.h"
#include "geometry/factory/PlaneModel.h"
#include "geometry/spatial/Quadtree.h"
#include "RenderableTexture.h"
#include "camera/Camera.h"
#include "Skybox.h"
#include "../graphics/shadows/DirLightShadowMap.h"

class WaterShader;
class Scene;

class Water {

public:
	Water(float height, WaterShader* waterShader);
	~Water();

	void drawWaterTexture(Scene* scene, Camera& cam, Skybox* skybox, DirLightShadowMap& dirLight);

	void beginReflectionRendering(Camera& cam);
	void endReflectionRendering(Camera& cam);

	void beginRefractionRendering();
	void endRefractionRendering();

	void updateWaves(float dt);

	// Call this when the window has resized
	void resize(int width, int height);
	Model& getModel();
	RenderableTexture* getReflection();
	RenderableTexture* getRefraction();
	float getHeight() const;
	WaterShader* getShader() const;

private:
	float m_height;
	ID3D11ShaderResourceView* m_srvs[4];
	RenderableTexture m_reflection;
	RenderableTexture m_refraction;
	std::unique_ptr<Model> m_waterPlane;
	DXTexture m_dudvMap;
	DXTexture m_normalMap;

	WaterShader* m_waterShader;

	DirectX::SimpleMath::Vector3 m_oldCamPos;
	DirectX::SimpleMath::Vector3 m_oldCamDirection;
};