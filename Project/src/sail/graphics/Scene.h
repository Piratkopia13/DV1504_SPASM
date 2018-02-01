#pragma once

#include <vector>
#include <memory>
#include <map>
#include "geometry/Model.h"
#include "Lights.h"
#include "shader/basic/CubeMapShader.h"
#include "Skybox.h"
#include "text/Text.h"
#include "../Application.h"
#include "geometry/spatial/Quadtree.h"
#include "renderer/DeferredRenderer.h"
#include "../utils/Timer.h"
#include "shader/basic/DepthShader.h"
//#include "../../game/level/Level.h"
//#include "../../game/ProjectileHandler.h"
//#include "../../game/objects/common/Object.h"

class ProjectileHandler;
class Object;
class Level;
class Scene {

public:
	Scene(const AABB& worldSize);
	~Scene();

	// Adds the specified model to the scene and it's quadtree
	// The model will only be drawn when its aabb is inside or intersecting the cameras frustum
	void addObject(Object* newObject);

	// Adds the specified text to the scene
	// This does not take ownership of the object
	void addText(Text* text);

	// Adds a skybox using the specified cube texture
	void addSkybox(const std::wstring& filename);

	// Draws the scene
	void draw(float dt, Camera& cam, Level* level, ProjectileHandler* projectiles);
	// Draws the HUD
	void drawHUD();

	// Resizes textures used
	// This has to be called on window  
	void resize(int width, int height);

	// Setup the DL for shadows
	void setShadowLight();

	// Return the lights
	Lights& getLights();
	// Return the deferred renderer
	DeferredRenderer& getDeferredRenderer();

	DirLightShadowMap& getDLShadowMap();

private:
	std::map<ShaderSet*, std::vector<Model*>> mapModelsToShaders(std::vector<Quadtree::Element*>& elements);

private:
	DeferredRenderer m_deferredRenderer;
	Timer m_timer;

	//std::map<ShaderSet*, std::vector<Model*>> m_mappedModels;
	std::vector<Object*> m_objects;

	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
	std::vector<Text*> m_texts;

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