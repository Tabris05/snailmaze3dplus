#ifdef NDEBUG
#define main WinMain
#endif

#include "engine.h"

int main() {
	Engine::run();
}