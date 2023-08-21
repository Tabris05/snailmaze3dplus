#include "engine.h"
#include "pqueue.h"

#include <cmath>
#include <array>
#include <chrono>
#include <unordered_map>
using std::abs, std::cos, std::pow, std::sin, std::atan2, std::sqrt, std::array, std::unordered_map, std::chrono::steady_clock, std::chrono::duration, std::chrono::seconds;
Engine::Snail::Snail() : m_position{ m_tileSize * 39.5, m_tileSize * 15.5 }, m_lastSawPlayer{steady_clock::now()} {}
Engine::Point<double> Engine::Snail::getPos() {
	return m_position;
}
void Engine::Snail::update() {
	if(canSeePlayer() || duration<double>(steady_clock::now() - m_lastSawPlayer) > seconds(30)) {
		const Point<double> pPos = m_player.getPos();
		findPath({(short)(pPos.x / m_tileSize), (short)(pPos.y / m_tileSize)});
		m_lastSawPlayer = steady_clock::now();
		m_hasDestination = true;
	}
	if (m_hasDestination) {
		float travelDistance = m_movSpeed * m_logic.getDt();
		Point<short> location = { m_position.x / m_tileSize, m_position.y / m_tileSize };
		Point<short> parent = m_parents.at(location);
		Point<float> direction = { (parent.x * m_tileSize + m_tileSize / 2.0) - m_position.x, (parent.y * m_tileSize + m_tileSize / 2.0) - m_position.y };
		if (abs(direction.x) >= travelDistance) {
			m_position.x += sign(direction.x) * travelDistance;
		}
		if (abs(direction.y) >= travelDistance) {
			m_position.y += sign(direction.y) * travelDistance;
		}
		if(abs(direction.x) < travelDistance && abs(direction.y) < travelDistance) {
			m_hasDestination = false;
		}
	}
	else {
		Point<short> newDest;
		do {
			newDest = { (short)(rand() % (m_mazeW * 2 + 1)), (short)(rand() % (m_mazeH * 2 + 1)) };
		} while (!m_maze[newDest.x][newDest.y]);
		findPath(newDest);
		m_hasDestination = true;
	}
}
bool Engine::Snail::canSeePlayer() {
	const Point<double> pPos = m_player.getPos();
	const float angle = atan2(pPos.y - m_position.y, pPos.x - m_position.x);
	const Point<double> direction{ cos(angle), sin(angle) };
	const double slope = direction.y / direction.x;
	const Point<double> stepSize{ sqrt(1 + slope * slope), sqrt(1 + (1 / slope) * (1 / slope)) };
	const Point<short> stepDir{ direction.x < 0 ? -1 : 1, direction.y < 0 ? -1 : 1 };
	Point<short> curCell{ m_position.x / m_tileSize, m_position.y / m_tileSize };
	const Point<double> startPos{
			direction.x < 0 ?
			m_position.x / m_tileSize - curCell.x :
			(curCell.x + 1) - m_position.x / m_tileSize,

			direction.y < 0 ?
			m_position.y / m_tileSize - curCell.y :
			(curCell.y + 1) - m_position.y / m_tileSize
	};
	Point<double> curLength{ startPos.x * stepSize.x, startPos.y * stepSize.y };
	double curDistance = 0;
	while (m_maze[curCell.x][curCell.y]) {
		if (curLength.x < curLength.y) {
			curCell.x += stepDir.x;
			curDistance = curLength.x;
			curLength.x += stepSize.x;
		}
		else {
			curCell.y += stepDir.y;
			curDistance = curLength.y;
			curLength.y += stepSize.y;
		}
	}
	return curDistance * m_tileSize > sqrt(pow(pPos.y - m_position.y, 2) + pow(pPos.x - m_position.x, 2));
}
void Engine::Snail::findPath(Engine::Point<short> start) {
	static constexpr array<Point<short>, 4> directions{ Point<short>{ -1, 0 }, Point<short>{ 1, 0 }, Point<short>{ 0, -1 }, Point<short>{ 0, 1 } };
	PQueue<Point<short>, double, hashPoint> pq;
	unordered_map<Point<short>, bool, hashPoint> visited;
	unordered_map<Point<short>, double, hashPoint> distances;
	m_parents = unordered_map<Point<short>, Point<short>, hashPoint>();
	Point<short> destination = { m_position.x / m_tileSize, m_position.y / m_tileSize };
	pq.push(start, 0.0);
	distances[start] = 0.0;
	m_parents[start] = start;
	while (!pq.empty()) {
		Point<short> cur = pq.pop();
		if (cur == destination) break;
		visited[cur] = true;
		for (const auto& [i, j] : directions) {
			Point<short> tmp{ cur.x + i, cur.y + j };
			if (isValid(tmp)) {
				if (!visited[tmp]) {
					distances[tmp] = DBL_MAX;
					pq.push(tmp, DBL_MAX);
				}
				if (distances[cur] + 1.0 < distances[tmp]) {
					distances[tmp] = distances[cur] + 1.0;
					m_parents[tmp] = cur;
					pq.updatePriority(tmp, abs(destination.x - tmp.x) + abs(destination.y - tmp.y) + distances[cur] + 1.0);
				}
			}
		}
	}
}
bool Engine::Snail::isValid(Engine::Point<short> point) {
	return point.x >= 0 && point.x < (m_mazeW * 2 + 1) && point.y >= 0 && point.y < (m_mazeH * 2 + 1) && m_maze[point.x][point.y];
}