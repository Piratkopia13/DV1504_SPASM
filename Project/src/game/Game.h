#pragma once

#include "../sail/Sail.h"
#include "PlayerCameraController.h"

class Game : public Application {

public:
	Game(HINSTANCE hInstance);
	~Game();

	virtual int run();
	virtual void processInput(float dt);
	virtual void update(float dt);
	virtual void render(float dt);
	virtual void resize(int width, int height);

private:
	PerspectiveCamera m_cam;
	OrthographicCamera m_hudCam;
	FlyingCameraController m_camController;
	Scene m_scene;
	std::unique_ptr<FbxModel> m_tessSphere;
	std::unique_ptr<FbxModel> m_sphere;
	std::unique_ptr<Model> m_plane;
	std::unique_ptr<Model> m_texturePlane;
	std::unique_ptr<Model> m_texturePlane2;

	// Stuff to show that the octree is culling models
	PerspectiveCamera m_quadtreeCam;
	RenderableTexture m_quadtreeCamTex;
	std::unique_ptr<Model> m_quadtreeCamtexPlane;

	SimpleColorShader m_colorShader;
	SimpleTextureShader m_texShader;
	SimpleTextureShader m_hudShader;
	DirectionalLightShader m_dirLightShader;
	MaterialShader m_matShader;
	ParticleShader m_particleShader;
	SailFont m_font;

	Timer m_timer;

	Text m_fpsText;
	Text m_debugText;
	Text m_debugCamText;
	Text m_debugParticleText;

	PlayerCameraController m_playerCamController;
	bool m_flyCam;

	std::unique_ptr<FbxModel> m_fbxModel;
	std::vector<Model*> m_modelCopies;


	// TEST REMOVE THIS
	std::vector<std::unique_ptr<Model>> models;

	std::unique_ptr<BillboardSystem> m_particleSystem;

};