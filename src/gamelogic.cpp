#include "engine.h"
#include <cmath>
#include <chrono>

using std::chrono::steady_clock, std::chrono::seconds, std::sqrt, std::pow, std::min;

void Engine::GameLogic::step() {
	m_dt = min(GetFrameTime(), m_maxLogicStep);
	switch (m_screen) {
		case MENU:
			menuLogic();
			break;
		case INSTRUCTIONS:
			instructionsLogic();
			break;
		case GAMEPLAY:
			gameplayLogic();
			break;
		case LEVEL_TRANSITION:
			transitionLogic();
			break;
		case GAMEOVER:
			gameOverLogic();
			break;
	}
}
void Engine::GameLogic::menuLogic() const {
	if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_S) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN)) m_cursor = !m_cursor;
	if (IsKeyPressed(KEY_ENTER)) {
		if (m_cursor) {
			PlayMusicStream(m_assetManager.m_gameplay);
			SetMousePosition(m_renderer.m_screenW / 2, m_renderer.m_screenH / 2);
			m_screen = GAMEPLAY;
		}
		else m_screen = INSTRUCTIONS;
	}
}
void Engine::GameLogic::instructionsLogic() const {
	if (IsKeyPressed(KEY_ENTER)) m_screen = MENU;
}
void Engine::GameLogic::gameplayLogic() const {
	UpdateMusicStream(m_assetManager.m_gameplay);
	if (m_completed) {
		m_completed = false;
		m_maze = MazeGenerator().generate();
		m_player = Player();
		m_snail = Snail();
	}
	else {
		m_player.update();
		m_snail.update();
		const Point<double> pPos{ m_player.getPos() };
		const Point<double> sPos{ m_snail.getPos() };
		if (short(pPos.x / m_tileSize) == m_mazeW * 2 - 1 && short(pPos.y / m_tileSize) == m_mazeH * 2 - 1) {
			m_completed = true;
			m_screen = LEVEL_TRANSITION;
		}
		else if (sqrt(pow(pPos.y - sPos.y, 2) + pow(pPos.x - sPos.x, 2)) < 25 * sqrt(2) + DBL_EPSILON) {
			m_completed = true;
			m_screen = GAMEOVER;
		}
	}
}
void Engine::GameLogic::transitionLogic() const {
	static bool ticking = false;
	static steady_clock::time_point tp;
	if (!ticking) {
		ticking = true;
		tp = steady_clock::now();
		StopMusicStream(m_assetManager.m_gameplay);
		PlaySound(m_assetManager.m_levelComplete);
	}
	if (steady_clock::now() - tp < seconds(3)) return;
	ticking = false;
	PlayMusicStream(m_assetManager.m_gameplay);
	SetMousePosition(m_renderer.m_screenW / 2, m_renderer.m_screenH / 2);
	m_screen = GAMEPLAY;
}
void Engine::GameLogic::gameOverLogic() const {
	static bool ticking = false;
	static steady_clock::time_point tp;
	if (!ticking) {
		ticking = true;
		tp = steady_clock::now();
		StopMusicStream(m_assetManager.m_gameplay);
	}
	if (steady_clock::now() - tp < seconds(3)) return;
	ticking = false;
	m_screen = MENU;
}
float Engine::GameLogic::getDt() const {
	return m_dt;
}