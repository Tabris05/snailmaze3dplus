#include "engine.h"
#include <cmath>
#include <raylib.h>
#include <unordered_map>

#include <iostream>

using std::unordered_map;

Engine::Player::Player() : m_position{ m_tileSize * 1.5, m_tileSize * 1.5 }, m_rot{ m_maze[2][1] ? 0.0 : 90.0 } {}
void Engine::Player::update() {
	updateRot();
	updatePos();
	checkCollision();
}
void Engine::Player::updateRot() {
	static bool justMoved = false;
	if (justMoved) justMoved = false;
	else {
		int mouseX = GetMouseX();
		m_rot += GetMouseDelta().x * m_sens * m_logic.getDt();
		// raylib can't handle resetting the mouse pos every frame so we have to do this terribleness
		if (mouseX < m_renderer.m_screenW / 3 || mouseX > m_renderer.m_screenW * 2 / 3) {
			SetMousePosition(m_renderer.m_screenW / 2, m_renderer.m_screenH / 2);
			justMoved = true;
		}
	}
	if (m_rot < 0) m_rot += 360;
	if (m_rot > 360) m_rot -= 360;
}
void Engine::Player::updatePos() {
	static const unordered_map<unsigned short, KeyboardKey> key{
		{ 0, KEY_W }, { 90, KEY_D }, { 180, KEY_S },  {270, KEY_A }
	};
	for (unsigned short angle = 0; angle < 360; angle += 90) {
		if (IsKeyDown(key.at(angle))) {
			m_position.x += cos((m_rot + angle) * m_toRad) * m_movSpeed * m_logic.getDt();
			m_position.y += sin((m_rot + angle) * m_toRad) * m_movSpeed * m_logic.getDt();
		}
	}
}
void Engine::Player::checkCollision() {
	if (!m_maze[short(m_position.x + m_size) / m_tileSize][short(m_position.y) / m_tileSize]) m_position.x = short((m_position.x + m_size) / m_tileSize) * m_tileSize - m_size;
	else if (!m_maze[short(m_position.x - m_size) / m_tileSize][short(m_position.y) / m_tileSize]) m_position.x = short((m_position.x - m_size) / m_tileSize + 1) * m_tileSize + m_size;
	if (!m_maze[short(m_position.x) / m_tileSize][short(m_position.y + m_size) / m_tileSize]) m_position.y = short((m_position.y + m_size) / m_tileSize) * m_tileSize - m_size;
	else if (!m_maze[short(m_position.x) / m_tileSize][short(m_position.y - m_size) / m_tileSize]) m_position.y = short((m_position.y - m_size) / m_tileSize + 1) * m_tileSize + m_size;
}
Engine::Point<double> Engine::Player::getPos() const {
	return m_position;
}
double Engine::Player::getRot() const {
	return m_rot;
}