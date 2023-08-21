#include "engine.h"
#include <chrono>
#include <vector>

using std::vector, std::chrono::steady_clock;

Engine::Maze Engine::MazeGenerator::generate() {
	srand(steady_clock::now().time_since_epoch().count());
	m_visited[0][0] = true;
	m_maze[1][1] = true;
	for (int x = 0; x < m_mazeW; x++) {
		for (int y = 0; y < m_mazeH; y++) {
			if (m_visited[x][y]) {
				mazeGenVisit(x, y);
			}
		}
	}
	return m_maze;
}
void Engine::MazeGenerator::mazeGenVisit(short x, short y) {
	if (!m_visited[x][y]) m_visited[x][y] = true;
	vector<Point<short>> validDirections;
	if (x - 1 >= 0 && !m_visited[x - 1][y]) {
		validDirections.push_back({ -1, 0 });
	}
	if (x + 1 < m_mazeW && !m_visited[x + 1][y]) {
		validDirections.push_back({ 1, 0 });
	}
	if (y - 1 >= 0 && !m_visited[x][y - 1]) {
		validDirections.push_back({ 0, -1 });
	}
	if (y + 1 < m_mazeH && !m_visited[x][y + 1]) {
		validDirections.push_back({ 0, 1 });
	}
	const size_t size = validDirections.size();
	if (size > 0) {
		const auto [movX, movY] = validDirections[rand() % size];
		m_maze[(x * 2 + 1) + movX][(y * 2 + 1) + movY] = true;
		m_maze[(x * 2 + 1) + movX * 2][(y * 2 + 1) + movY * 2] = true;
		mazeGenVisit(x + movX, y + movY);
	}
	else if (rand() % 3 == 0) {
		if (x - 1 >= 0) {
			validDirections.push_back({ -1, 0 });
		}
		if (x + 1 < m_mazeW) {
			validDirections.push_back({ 1, 0 });
		}
		if (y - 1 >= 0) {
			validDirections.push_back({ 0, -1 });
		}
		if (y + 1 < m_mazeH) {
			validDirections.push_back({ 0, 1 });
		}
		const auto [movX, movY] = validDirections[rand() % validDirections.size()];
		m_maze[(x * 2 + 1) + movX][(y * 2 + 1) + movY] = true;
		m_maze[(x * 2 + 1) + movX * 2][(y * 2 + 1) + movY * 2] = true;
	}
}
