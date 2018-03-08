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

	/*VIB REMOVAL*/
	auto& gamePad = m_app->getInput().getGamePad();
	for (int u = 0; u < 4; u++)
		gamePad.SetVibration(u, 0, 0);

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
	
	addScore();
	std::vector<bool> used;
	for (size_t i = 0; i < m_info->getPlayers().size(); i++) {
		m_ready.push_back(false);
		used.push_back(false);
	}

	Score* score = &m_info->getScore();
	std::vector<GameInfo::Player> playerName;
	std::vector<Score::PlayerStats> playerStats;
	
	float oldMax = 1000000;
	float oldDeaths = 0;


	for (size_t i = 0; i < score->getSize(); i++) {
		float tempMaxKills = -1;
		float tempDeaths = -1;
		size_t index = 0;
		for (size_t u = 0; u < score->getSize(); u++) {
			if (used[u])
				continue;
			int kills = score->getPlayerStats(u).kills;
			int deaths = score->getPlayerStats(u).deaths;
			// om fler kills
			if (tempMaxKills < kills) {
				index = u;
				tempMaxKills = kills;
				tempDeaths = deaths;
			}
			// om lika kills & mindre deaths
			else if (tempMaxKills == kills && tempDeaths >= deaths) {
				index = u;
				tempMaxKills = kills;
				tempDeaths = deaths;

			}
		}
		if (used[index])
			continue;
		used[index] = true;
		oldMax = tempMaxKills;
		oldDeaths = score->getPlayerStats(index).deaths;
		playerName.push_back(m_info->getPlayers()[index]);
		playerStats.push_back(score->getPlayerStats(index));
	}

	


	m_topLine.text.push_back(new MenuText("name"));
	m_topLine.text.push_back(new MenuText("kills"));
	m_topLine.text.push_back(new MenuText("deaths"));
	/*if (m_info->convertedGameSettings.gamemode == 0) {
		m_topLine.text.push_back(new MenuText("captures"));
	}
	m_topLine.text.push_back(new MenuText("score"));*/

	for (size_t i = 0; i < m_topLine.text.size(); i++) {
		m_scene.addObject(m_topLine.text[i]);
	}


	for (size_t i = 0; i < playerStats.size(); i++) {
		ScoreLine temp;
		temp.text.push_back(new MenuText(playerName[i].currentProfile->getName()));
		temp.text.push_back(new MenuText(std::to_string(playerStats[i].kills)));
		temp.text.push_back(new MenuText(std::to_string(playerStats[i].deaths)));
		/*if (m_info->convertedGameSettings.gamemode == 0) {
			temp.text.push_back(new MenuText(std::to_string(playerStats[i].pointsCaptured)));
		}
		temp.text.push_back(new MenuText(std::to_string((int)playerStats[i].gamePoints)));*/
		m_scoreLine.push_back(temp);

		for (size_t u = 0; u < m_scoreLine[i].text.size(); u++) {
			m_scoreLine[i].text[u]->setColor(m_info->getDefaultColor(playerName[i].color,playerName[i].hue) * ((m_info->convertedGameSettings.teams[playerName[i].team].winner) ? 3:1));
			m_scene.addObject(m_scoreLine[i].text[u]);
		}
	}
	for (size_t i = 0; i < m_info->convertedGameSettings.teams.size(); i++) {
		if (m_info->convertedGameSettings.teams[i].winner) {
			if (m_info->convertedGameSettings.gamemode == 1) {
				m_winner.text.push_back(new MenuText(playerName[0].currentProfile->getName() + " wins the game"));
			}
			else {
				m_winner.text.push_back(new MenuText("Team " + std::to_string(i+1) + " Wins the game"));

			}
			m_winner.text[0]->setColor(m_info->convertedGameSettings.teams[i].color * 2.0);
			m_winner.text[0]->setSize(4.0f);
			m_scene.addObject(m_winner.text[0]);
			break;
		}
	}

	setPositions();
	

	m_playerCamController->setUseExtraZ(false);
	m_playerCamController->setTarget(Vector3(0, 0, 1));
	m_playerCamController->setOffset(Vector3(0, 0, 0));
	m_playerCamController->setMoving(false);
	m_playerCamController->setPosition(Vector3(0, 0, -5));
	m_playerCamController->setFollowSpeed(8);

}
ScoreState::~ScoreState() {
	for (size_t i = 0; i < m_scoreLine.size(); i++) {
		m_scoreLine[i].clear();
	}
	m_topLine.clear();
	m_winner.clear();
}



// Process input for the state
bool ScoreState::processInput(float dt) {
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
		int menu = 0;
		int back = 0;
		int pressed = 0;
		int spacePressed = 0;

		if (i == keyboardPort) {
			
			if (kbTracker.pressed.Space) {
				a = 1;
				pressed = 1;
				spacePressed = 1;
			}
			if (kbTracker.pressed.Tab) {
				b = 1;
				pressed = 1;
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
			if (padTracker.menu == GamePad::ButtonStateTracker::PRESSED) {
				menu = 1;
				pressed = 1;
			}
			
		}
		if (pressed) {

			if (a || b || menu) {
				
				
				for (size_t u = 0; u < m_info->getPlayers().size(); u++) {
					if (i == m_info->getPlayers()[u].port) {
						m_ready[u] = true;
						break;
					}
				}

				size_t r = 0;
				for (size_t u = 0; u < m_ready.size(); u++) {
					if (m_ready[i])
						r++;
				}
				if(r == m_ready.size())
					exitScoreBoard();

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

void ScoreState::addScore() {

	for (size_t i = 0; i < m_info->getPlayers().size(); i++) {
		GameInfo::Player* playe = &m_info->getPlayers()[i];
		Score::PlayerStats stats = m_info->getScore().getPlayerStats(i);
		playe->currentProfile->addKills(stats.kills);
		playe->currentProfile->addDeaths(stats.deaths);
		if (m_info->convertedGameSettings.teams[playe->team].winner) {
			playe->currentProfile->addWin();
		}
		else {
			playe->currentProfile->addLoss();
		}
		
		

	}


}

void ScoreState::setPositions() {
	Vector3 position(0, 0, 0);
	Vector3 growth(0, -1, 0);
	Vector3 right(1, 0, 0);

	size_t size = m_scoreLine.size();
	float halfSizeY = (float)size*0.5f;


	float xSpace = 2.1f;
	float ySpace = 0.5f;

	m_winner.text[0]->setPosition(position - 
	growth * ((float)size + 1.0f) * ySpace);
	float halfSizeX = (float)(m_topLine.text.size()-1) * 0.5f;
	for (size_t i = 0; i < m_topLine.text.size(); i++) {

		m_topLine.text[i]->setPosition(position - 
			growth * ((float)size - 1.0f) * ySpace + 
			right * (i - halfSizeX) * xSpace);

		m_topLine.text[i]->setSize(2.3f);
	}

	for (size_t u = 0; u < size; u++) {
		
		halfSizeX = (float)(m_scoreLine[u].text.size()-1) * 0.5f;
		for (size_t i = 0; i < m_scoreLine[u].text.size(); i++) {
			
			m_scoreLine[u].text[i]->setPosition(position + 
				growth * (u - halfSizeY+0.7f) * ySpace + 
				right * (i - halfSizeX) * xSpace);

			m_scoreLine[u].text[i]->setSize(u == 0 ? 1.8f:1.6f);

		}

	

	}





}

void ScoreState::exitScoreBoard() {
	requestStackPop();
	requestStackPush(States::MainMenu);
}
