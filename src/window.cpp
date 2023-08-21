#include "engine.h"
#include <raylib.h>

#ifndef NDEBUG
#define ToggleFullscreen()
#endif
Engine::Window::Window() {
	InitWindow(0, 0, "Snail Maze 3D Plus");
	InitAudioDevice();
	SetWindowState(FLAG_VSYNC_HINT);
	ToggleFullscreen();
	HideCursor();
}

Engine::Window::~Window() {
	CloseAudioDevice();
	CloseWindow();
}