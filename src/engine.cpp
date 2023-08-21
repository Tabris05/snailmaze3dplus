#include "engine.h"
#include <raylib.h>

void Engine::run() {
	while (!WindowShouldClose()) {
		m_logic.step();
		m_renderer.step();
	}
}