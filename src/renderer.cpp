#include "engine.h"
#include <raylib.h>
#include <cmath>
#include <utility>
#include <algorithm>

#ifdef NDEBUG
#define DrawFPS(x, y)
#endif

using std::abs, std::cos, std::min, std::tan, std::pair, std::sort, std::atan2;

Engine::Renderer::Renderer() :
	m_screenW(GetScreenWidth()),
	m_screenH(GetScreenHeight()),
	m_resScaleX(m_screenW / 1920.0),
	m_resScaleY(m_screenH / 1080.0),
	m_distToPlane(m_screenW / 2 / tan(m_fov / 2 * m_toRad)),
	m_mapTileSize(10.0 * m_resScaleY),
	m_mapOffset(25.0 * m_resScaleY),
	m_mapScale(10.0 * m_resScaleY),
	m_zBuffer(new Renderable[m_screenW + 2]) {}
Engine::Renderer::~Renderer() {
	delete[] m_zBuffer;
}
void Engine::Renderer::step() const {
	BeginDrawing();
	switch (m_screen) {
		case MENU:
			renderMenu();
			break;
		case INSTRUCTIONS:
			renderInstructions();
			break;
		case GAMEPLAY:
			renderGameplay();
			break;
		case LEVEL_TRANSITION:
			renderTransition();
			break;
		case GAMEOVER:
			renderGameOver();
			break;
	}
	DrawFPS(50, 50);
	EndDrawing();
}
void Engine::Renderer::renderMenu() const {
	static const float width = m_assetManager.m_logo.width * 7 * m_resScaleX;
	static const float height = m_assetManager.m_logo.height * 7 * m_resScaleY;
	ClearBackground(BLACK);
	DrawTexturePro(m_assetManager.m_logo, { 0, 0, (float)m_assetManager.m_logo.width, (float)m_assetManager.m_logo.height }, { m_screenW / 2.0f, 100, width, height }, { width / 2, 0 }, 0, WHITE);
	drawText("PLAY", m_screenW / 2.0f - 200 * m_resScaleX, m_screenH / 2.0f + 200 * m_resScaleY, 50, LEFT);
	drawText("INSTRUCTIONS", m_screenW / 2.0f - 200 * m_resScaleX, m_screenH / 2.0f + 300 * m_resScaleY, 50, LEFT);
	drawText(">", m_screenW / 2.0f - 255 * m_resScaleX, m_screenH / 2.0f + (m_cursor ? 200 : 300) * m_resScaleY, 50, LEFT);
}
void Engine::Renderer::renderInstructions() const {
	ClearBackground(BLACK);
	drawText("W: MOVE FORWARD", m_screenW / 2.0f, m_screenH / 2.0f - 225 * m_resScaleY - 100, 50);
	drawText("S: MOVE BACKWARD", m_screenW / 2.0f, m_screenH / 2.0f - 150 * m_resScaleY - 100, 50);
	drawText("A: STRAFE LEFT", m_screenW / 2.0f, m_screenH / 2.0f - 75 * m_resScaleY - 100, 50);
	drawText("D: STRAFE RIGHT", m_screenW / 2.0f, m_screenH / 2.0f - 100, 50);
	drawText("MOUSE X-AXIS: LOOK", m_screenW / 2.0f, m_screenH / 2.0f + 75 * m_resScaleY - 100, 50);
	drawText("ENTER: SELECT", m_screenW / 2.0f, m_screenH / 2.0f + 150 * m_resScaleY - 100, 50);
	drawText(">BACK", m_screenW / 2.0f, m_screenH / 2.0f + 300 * m_resScaleY, 50);
}
void Engine::Renderer::renderGameplay() const {
	static const Point<float> goalSpriteSize = { m_assetManager.m_goal.width, m_assetManager.m_goal.height };
	static const Point<float> snailSpriteSize = { m_assetManager.m_snail.width, m_assetManager.m_snail.height };
	ClearBackground({ 0, 170, 255, 255 });
	DrawRectangle(0, 0, m_screenW, m_screenH / 2, {0, 0, 0, 255});
	for (short i = 0; i < m_screenW; i++) m_zBuffer[i] = calcWall(i);
	m_zBuffer[m_screenW] = calcSprite(m_goal, 20, GOAL);
	m_zBuffer[m_screenW + 1] = calcSprite(m_snail.getPos(), 40, SNAIL);
	sort(m_zBuffer, m_zBuffer + m_screenW + 2);
	for (size_t i = 0; i < m_screenW + 2; i++) {
		const Renderable cur = m_zBuffer[i];
		if (cur.m_type == WALL) {
			const unsigned char color = min((double)255, 0.8 * 255.0 * m_screenH / cur.m_distance / m_tileSize + 255 * 0.2);
			const float top = cur.m_screenY - cur.m_height / 2;
			const float bottom = cur.m_screenY + cur.m_height / 2;
			DrawLine(cur.m_screenX, top, cur.m_screenX, bottom, { 0, 0, color, 255 });
		}
		else if(cur.m_type == GOAL) {
			DrawTexturePro(m_assetManager.m_goal, { 0, 0, goalSpriteSize.x, goalSpriteSize.y }, { cur.m_screenX, cur.m_screenY, cur.m_width, cur.m_height }, { cur.m_width / 2.0f, cur.m_height / 2.0f }, 0, WHITE);
		}
		else if (cur.m_type == SNAIL) {
			DrawTexturePro(m_assetManager.m_snail, { 0, 0, snailSpriteSize.x, snailSpriteSize.y }, { cur.m_screenX, cur.m_screenY + (float)m_resScaleY * 7, cur.m_width, cur.m_height }, { cur.m_width / 2.0f, cur.m_height / 2.0f }, 0, WHITE);
		}
	}
}
void Engine::Renderer::renderTransition() const {
	ClearBackground(BLACK);
	drawText("LEVEL COMPLETE!", m_screenW / 2.0f, m_screenH / 2.0f, 50);
}
void Engine::Renderer::renderGameOver() const {
	ClearBackground(BLACK);
	drawText("THE SNAIL FEASTS...", m_screenW / 2.0f, m_screenH / 2.0f, 50);
}
Engine::Renderer::Renderable Engine::Renderer::calcWall(short i) const {
	const double pAngle = m_player.getRot();
	const Point<double> pPos = m_player.getPos();
	const double angle = atan2(i - m_screenW / 2.0, m_distToPlane) / m_toRad + pAngle;
	const Point<double> direction{ cos(angle * m_toRad), sin(angle * m_toRad) };
	const double slope = direction.y / direction.x;
	const Point<double> stepSize{ sqrt(1 + slope * slope), sqrt(1 + (1 / slope) * (1 / slope)) };
	const Point<short> stepDir{ direction.x < 0 ? -1 : 1, direction.y < 0 ? -1 : 1 };
	Point<short> curCell{ pPos.x / m_tileSize, pPos.y / m_tileSize };
	const Point<double> startPos{
			direction.x < 0 ?
			pPos.x / m_tileSize - curCell.x :
			(curCell.x + 1) - pPos.x / m_tileSize,

			direction.y < 0 ?
			pPos.y / m_tileSize - curCell.y :
			(curCell.y + 1) - pPos.y / m_tileSize
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
	return { (float)i, m_screenH / 2.0f, 1, min((float)m_screenH, (float)(m_screenH / curDistance / cos((pAngle - angle) * m_toRad))), curDistance * m_tileSize, WALL };
}
Engine::Renderer::Renderable Engine::Renderer::calcSprite(Engine::Point<double> spritePos, double spriteSize, Engine::Renderer::Type type) const {
	const Point<double> pPos = m_player.getPos();
	const double dist = sqrt(pow(spritePos.y - pPos.y, 2) + pow(spritePos.x - pPos.x, 2));
	float angle = atan2(spritePos.y - pPos.y, spritePos.x - pPos.x) / m_toRad - m_player.getRot();
	if (angle < -180) angle += 360;
	const float size = spriteSize * m_screenH / dist / cos(angle * m_toRad);
	return { (float)(tan(angle * m_toRad) * m_distToPlane + m_screenW / 2.0f), m_screenH / 2.0f, size, size, dist, abs(angle) < 90 ? type : NONE };
}
void Engine::Renderer::drawText(const char* text, float x, float y, float size, Justification just) const {
	float offset = 0;
	if (just == CENTER) offset = MeasureTextEx(m_assetManager.m_snailFont, text, m_screenH * size / 1080, 1).x / 2;
	DrawTextEx(m_assetManager.m_snailFont, text, { x - offset, y - m_screenH * size / 1080 / 2 }, m_screenH * size / 1080, 1, WHITE);
}