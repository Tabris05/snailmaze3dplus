#ifndef ENGINE_H
#define ENGINE_H

#include <array>
#include <chrono>
#include <utility>
#include <unordered_map>

#include <raylib.h>

class Engine {
	public:
		static void run();
	private:
		static constexpr short m_mazeW = 20;
		static constexpr short m_mazeH = 8;
		static constexpr short m_tileSize = 50;
		static constexpr double m_toRad = PI / 180.0;
		static constexpr float m_maxLogicStep = 1.0f / 60.0f + FLT_EPSILON;

		using Maze = std::array<std::array<bool, m_mazeH * 2 + 1>, m_mazeW * 2 + 1>;
		
		enum Screen {
			MENU,
			INSTRUCTIONS,
			GAMEPLAY,
			LEVEL_TRANSITION,
			GAMEOVER
		};
		
		static inline Maze m_maze;
		static inline bool m_cursor = true;
		static inline bool m_completed = true;
		static inline Screen m_screen = MENU;

		template <typename T>
		struct Point {
			T x, y;
			bool operator==(const Point<T>& other) const {
				return x == other.x && y == other.y;
			}
			bool operator!=(const Point<T>& other) const {
				return !(*this == other);
			}
			
		};
		class MazeGenerator {
		public:
			Maze generate();
		private:
			using Visitlist = std::array<std::array<bool, m_mazeH>, m_mazeW>;
			Visitlist m_visited;
			Maze m_maze;
			void mazeGenVisit(short x, short y);
		};
		static inline const struct Window {
			Window();
			~Window();
		} m_window;
		static inline const struct AssetManager {
			AssetManager();
			~AssetManager();
			const Texture m_goal;
			const Texture m_logo;
			const Texture m_snail;
			const Font m_snailFont;
			const Music m_gameplay;
			const Sound m_levelComplete;
		} m_assetManager;
		static inline const class Renderer {
		public:
			Renderer();
			~Renderer();
			void step() const;
			const short m_screenW;
			const short m_screenH;
		private:
			static constexpr Point<double> m_goal = { m_tileSize * 39.5, m_tileSize * 15.5 };
			static constexpr double m_fov = 90;
			const double m_resScaleX;
			const double m_resScaleY;
			const double m_distToPlane;
			const short m_mapTileSize;
			const short m_mapOffset;
			const short m_mapScale;
			enum Justification {
				LEFT,
				CENTER
			};
			enum Type {
				WALL,
				GOAL,
				SNAIL,
				NONE
			};
			struct Renderable {
				float m_screenX;
				float m_screenY;
				float m_width;
				float m_height;
				double m_distance;
				Type m_type = NONE;
				bool operator<(const Renderable& rhs) const {
					return rhs.m_distance < m_distance;
				}
			};
			Renderable* const m_zBuffer;
			void renderMenu() const;
			void renderInstructions() const;
			void renderGameplay() const;
			void renderTransition() const;
			void renderGameOver() const;
			Renderable calcWall(short i) const;
			Renderable calcSprite(Point<double> spritePos, double spriteSize, Type type) const;
			void drawText(const char* text, float x, float y, float size, Justification just = CENTER) const;
		} m_renderer;
		static inline class GameLogic {
		public:
			void step();
			float getDt() const;
		private:
			float m_dt;
			void menuLogic() const;
			void instructionsLogic() const;
			void gameplayLogic() const;
			void transitionLogic() const;
			void gameOverLogic() const;
		} m_logic;
		static inline class Player {
		public:
			Player();
			void update();
			Point<double> getPos() const;
			double getRot() const;
		private:
			static constexpr double m_movSpeed = 75;
			static constexpr double m_sens = 15;
			static constexpr short m_size = 2;
			Point<double> m_position;
			double m_rot;
			void updateRot();
			void updatePos();
			void checkCollision();

		} m_player;
		static inline class Snail {
			public:
				Snail();
				Point<double> getPos();
				void update();
			private:
				static constexpr double m_movSpeed = 80;
				bool m_hasDestination = false;
				std::chrono::steady_clock::time_point m_lastSawPlayer;
				struct hashPoint {
					size_t operator()(const Point<short>& p) const {
						auto hash1 = std::hash<short>{}(p.x);
						auto hash2 = std::hash<short>{}(p.y);

						if (hash1 != hash2) {
							return hash1 ^ hash2;
						}
						return hash1;
					}
				};
				std::unordered_map<Point<short>, Point<short>, hashPoint> m_parents;
				Point<double> m_position;
				bool canSeePlayer();
				bool isValid(Point<short> point);
				void findPath(Point<short> destination);
		} m_snail;

		static bool inRange(auto val, auto low, auto high) {
			return low <= val && high >= val;
		}
		template<typename T>
		static short sign(T val) {
			return val < (T)0 ? -1 : 1;
		}
};

#endif