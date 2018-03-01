#include "ScoreState.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

ScoreState::ScoreState(StateStack& stack)
	: State(stack)
	, m_cam(60.f, 1280.f / 720.f, 0.1f, 1000.f)
	, m_fpsText(&m_font, L"")
	, m_debugCamText(&m_font, L"")
	, m_scene(AABB(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f)))
{
	// Get the Application instance
	m_app = Application::getInstance();
	m_info = GameInfo::getInstance();
	// Set up camera with controllers
	m_playerCamController = std::make_unique<PlayerCameraController>(&m_cam);

	// Set up the scene
	//m_scene.addSkybox(L"skybox_space_512.dds");
	// Add a directional light
	Vector3 color(0.9f, 0.9f, 0.9f);
	Vector3 direction(0.4f, -0.6f, 1.0f);
	direction.Normalize();
	m_scene.setUpDirectionalLight(Lights::DirectionalLight(color, direction));

	// Set up HUD texts
	m_debugCamText.setPosition(Vector2(0.f, 20.f));
	// Add texts to the scene
	m_scene.addText(&m_fpsText);
#ifdef _DEBUG
	m_scene.addText(&m_debugCamText);
#endif

	;

	m_info->addPlayer({&m_info->getProfiles()[0],0,0,0,0,0,0,0,0 });
	m_info->addPlayer({ &m_info->getProfiles()[1],3,0,0,0,0,0,0,0 });
	m_info->addPlayer({ &m_info->getProfiles()[2],1,1,0,0,0,0,0,0 });
	m_info->addPlayer({ &m_info->getProfiles()[3],2,1,0,0,0,0,0,0 });




	m_info->getScore().addKill(0);
	m_info->getScore().addKill(0);
	m_info->getScore().addKill(0);
	m_info->getScore().addKill(0);
	m_info->getScore().addKill(1);
	m_info->getScore().addKill(1);
	m_info->getScore().addKill(2);
	m_info->getScore().addKill(2);
	m_info->getScore().addKill(2);
	m_info->getScore().addKill(3);
	m_info->getScore().addPoints(0, 40);
	m_info->getScore().addPoints(1, 20);
	m_info->getScore().addPoints(2, 30);
	m_info->getScore().addPoints(3, 10);



	int nr = m_info->convertedGameSettings.teams.size();

	Score* score = &m_info->getScore();
	std::vector<GameInfo::Player> playerName;
	std::vector<Score::PlayerStats> playerStats;

	float oldMax = 1000000;
	for (size_t i = 0; i < score->getSize(); i++) {
		float maxScore = 0;
		size_t index = 0;
		for (size_t u = 0; u < score->getSize(); u++) {
			if (maxScore < score->getPlayerStats(u).gamePoints && score->getPlayerStats(u).gamePoints < oldMax) {
				index = u;
				maxScore = score->getPlayerStats(u).gamePoints;
			}
		}
		oldMax = maxScore;
		playerName.push_back(m_info->getPlayers()[index]);
		playerStats.push_back(score->getPlayerStats(index));
	}


	for (size_t i = 0; i < 2; i++) {

	}

	//for (size_t i = 0; i < playerStats.size(); i++) {
	//	m_scoreLine.push_back({
	//		new MenuText(playerStats[i].),

	//		
	//		
	//		});




	//}













	m_playerCamController->setUseExtraZ(false);
	m_playerCamController->setTarget(Vector3(0, 0, 1));
	m_playerCamController->setOffset(Vector3(0, 0, 0));
	m_playerCamController->setMoving(false);
	m_playerCamController->setPosition(Vector3(0, 0, 0));
	m_playerCamController->setFollowSpeed(8);

}
ScoreState::~ScoreState() {
	for (size_t i = 0; i < m_scoreLine.size(); i++) {
		m_scoreLine[i].clear();
	}
}



// Process input for the state
bool ScoreState::processInput(float dt) {
	static int ready = 0;



	const Keyboard::KeyboardStateTracker& kbTracker = m_app->getInput().getKbStateTracker();
	static int keyboardPort = 0;
	if (kbTracker.pressed.D1) {
		keyboardPort = 0;
	}
	if (kbTracker.pressed.D2) {
		keyboardPort = 1;
	}
	if (kbTracker.pressed.D3) {
		keyboardPort = 2;
	}
	if (kbTracker.pressed.D4) {
		keyboardPort = 3;
	}


	for (int i = 0; i < 4; i++) {
		const GamePad::State& padState = m_app->getInput().getGamePadState(i);
		const GamePad::ButtonStateTracker& padTracker = m_app->getInput().getGpStateTracker(i);

		int a = 0;
		int b = 0;
		int down = 0;
		int up = 0;
		int left = 0;
		int right = 0;
		int menu = 0;
		int back = 0;
		int pressed = 0;
		int spacePressed = 0;

		if (i == keyboardPort) {
			if (kbTracker.pressed.W) {
				up = 1;
				pressed = 1;
			}
			if (kbTracker.pressed.S) {
				down = 1;
				pressed = 1;
			}
			if (kbTracker.pressed.D) {
				right = 1;
				pressed = 1;
			}
			if (kbTracker.pressed.A) {
				left = 1;
				pressed = 1;
			}
			if (kbTracker.pressed.Space) {
				a = 1;
				pressed = 1;
				spacePressed = 1;
			}
			if (kbTracker.pressed.Tab) {
				b = 1;
				pressed = 1;
			}
			if (kbTracker.pressed.F) {
				static int camP = 0;
				camP++;
				if (camP > 3)
					camP = 0;
				if (camP == 0)
					m_playerCamController->setPosition(Vector3(0, 0, 0));
				if (camP == 1)
					m_playerCamController->setPosition(Vector3(0, 15, 0));
				if (camP == 2)
					m_playerCamController->setPosition(Vector3(10, 15, 10));
				if (camP == 3)
					m_playerCamController->setPosition(Vector3(-10, 15, -10));

			}
		}


		if (padState.IsConnected()) {

			if (padTracker.a == GamePad::ButtonStateTracker::PRESSED) {
				a = 1;
				pressed = 1;
			}
			if (padTracker.b == GamePad::ButtonStateTracker::PRESSED || padTracker.back == GamePad::ButtonStateTracker::PRESSED) {
				b = 1;
				pressed = 1;
			}
			if (padTracker.dpadUp == GamePad::ButtonStateTracker::PRESSED || padTracker.leftStickUp == GamePad::ButtonStateTracker::PRESSED) {
				up = 1;
				pressed = 1;
			}
			if (padTracker.dpadDown == GamePad::ButtonStateTracker::PRESSED || padTracker.leftStickDown == GamePad::ButtonStateTracker::PRESSED) {
				down = 1;
				pressed = 1;
			}
			if (padTracker.dpadRight == GamePad::ButtonStateTracker::PRESSED || padTracker.leftStickRight == GamePad::ButtonStateTracker::PRESSED) {
				right = 1;
				pressed = 1;
			}
			if (padTracker.dpadLeft == GamePad::ButtonStateTracker::PRESSED || padTracker.leftStickLeft == GamePad::ButtonStateTracker::PRESSED) {
				left = 1;
				pressed = 1;
			}
		}
		if (pressed) {

			if (a) {




				requestStackClear();
				requestStackPush(States::MainMenu);

			}




		}
	}
	return true;
}

// Process window resizing for the state
bool ScoreState::resize(int width, int height) {
	m_cam.resize(width, height);
	m_scene.resize(width, height);

	return true;
}

// Updates the state
bool ScoreState::update(float dt) {

	// Update HUD texts
	m_fpsText.setText(L"FPS: " + std::to_wstring(m_app->getFPS()));

	auto& camPos = m_cam.getPosition();
	m_debugCamText.setText(L"Camera @ " + Utils::vec3ToWStr(camPos) + L" Direction: " + Utils::vec3ToWStr(m_cam.getDirection()));

	m_playerCamController->update(dt);

	return true;
}
// Renders the state
bool ScoreState::render(float dt) {
	// Clear back buffer
	m_app->getDXManager()->clear(DirectX::Colors::Black);

	// Draw the scene
	m_scene.draw(dt, m_cam);

	// Draw HUD
	m_scene.drawHUD();

	return true;
}
