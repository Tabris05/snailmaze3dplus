#include "engine.h"
#include <raylib.h>

Engine::AssetManager::AssetManager() :
	m_goal(LoadTexture("assets/goal.png")),
	m_logo(LoadTexture("assets/logo.png")),
	m_snail(LoadTexture("assets/snail.png")),
	m_snailFont(LoadFont("assets/snailfont.ttf")),
	m_gameplay(LoadMusicStream("assets/gameplay.mp3")),
	m_levelComplete(LoadSound("assets/levelend.ogg")) {}
Engine::AssetManager::~AssetManager() {
	UnloadTexture(m_goal);
	UnloadTexture(m_logo);
	UnloadFont(m_snailFont);
	UnloadSound(m_levelComplete);
	UnloadMusicStream(m_gameplay);
}