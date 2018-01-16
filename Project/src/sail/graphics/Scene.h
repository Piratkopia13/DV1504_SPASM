#pragma once

#include <vector>
#include <memory>
#include <map>
#include "geometry/Model.h"
#include "Lights.h"
#include "Water.h"
#include "shader/basic/WaterShader.h"
#include "shader/basic/CubeMapShader.h"
#include "Skybox.h"
#include "text/Text.h"
#include "../Application.h"
#include "geometry/spatial/Quadtree.h"
#include "renderer/DeferredRenderer.h"
#include "../utils/Timer.h"
#include "shader/basic/DepthShader.h"

class Scene {

public:
	Scene(const AABB& worldSize);
	~Scene();

	// Adds the specified model to the scene and it's quadtree
	// The model will only be drawn when its aabb is inside or intersecting the cameras frustum
	void addModelViaQuadtree(Model* model);

	// Adds the specified text to the scene
	// This does not take ownership of the object
	void addText(Text* text);

	// Adds water at the specified y-level
	void addWater(float height);

	// Adds a skybox using the specified cube texture
	void addSkybox(const std::wstring& filename);

	// Draws the scene
	void draw(float dt, Camera& cam);
	// Draws the HUD
	void drawHUD();

	// Resizes textures used
	// This has to be called on window  
	// resize when rendering water
	void resize(int width, int height);

	// Setup the DL for shadows
	void setShadowLight();

	// Return the Water
	Water* getWater() const;
	// Return the lights
	Lights& getLights();
	// Return the Quadtree
	Quadtree& getQuadtree();
	// Return the deferred renderer
	DeferredRenderer& getDeferredRenderer();

	DirLightShadowMap& getDLShadowMap();

private:
	std::map<ShaderSet*, std::vector<Model*>> mapModelsToShaders(std::vector<Quadtree::Element*>& elements);

private:
	Quadtree m_quadtree;
	DeferredRenderer m_deferredRenderer;
	Timer m_timer;

	//std::map<ShaderSet*, std::vector<Model*>> m_mappedModels;

	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::vector<Text*> m_texts;

	// Water
	std::unique_ptr<WaterShader> m_waterShader;
	std::unique_ptr<Water> m_water;

	// Lights
	Lights m_lights;

	// Skybox
	std::unique_ptr<Skybox> m_skybox;
	std::unique_ptr<CubeMapShader> m_cubeMapShader;

	DepthShader m_depthShader;

	// Shadow maps
	DirLightShadowMap m_dirLightShadowMap;


	// Camera rotation
	float m_rotation;
};