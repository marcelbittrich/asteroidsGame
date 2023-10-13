#pragma once

#include <vector>
#include <list>

#include "SDL_ttf.h"
#include "SDL_stdinc.h"

#include "gameobjects.hpp"
#include "menu.hpp"
#include "gamesave.hpp"
#include "gamestate.hpp"
#include "background.hpp"
#include "collisionhandler.h"

#define PI 3.14159265359

// Gameplay parameters
const int STARTING_LIVES = 3;
const int STARTING_BOMB_COUNT = 0;
// One in ... for dropping a bomb when destroing astroids.
const int BOMB_SPAWN_ON_SCORE = 50;
const float ASTEROID_SPAWN_DELTATIME = 3.0f;
const float ASTEROID_SPAWN_SPEED_MULTI = 0.03f;

class Game
{
public:
	Game();
	~Game();
	void Init(const char* title, int xpos, int ypos, int m_width, int m_height, bool fullscreen);
	void HandleEvents();
	void Update();
	void Render();
	void Clean();
	void Reset();

	static enum class GameState
	{
		IN_MENU,
		IN_GAME,
		RESET,
		PAUSE
	};

	void PrintPerformanceInfo(Uint32 updateTime, Uint32 renderTime, Uint32 loopTime, Uint32 frameTime);
	bool GetIsRunning() const { return isRunning; }
	
	static void IncreaseScore() { score++; }
	static int GetScore() { return score; }
	static void DecreseLife() { life--; }
	GameState GetState() const { return gameState; }

	static void changeStateToGame() { gameState = GameState::IN_GAME; }
	static void changeStateToReset() { gameState = GameState::RESET; }
	static void changeStateToPause() { gameState = GameState::PAUSE; }
	static void exitGame() { isRunning = false; }
private:
	inline static GameState gameState = GameState::IN_MENU;
	inline static bool isRunning = true;
	// Game window values
	int windowWidth;
	int windowHeight;
	SDL_Window* window;
	SDL_Renderer* renderer;
	void InitWindow(const char* title, int xpos, int ypos, int m_width, int m_height, bool fullscreen);

	// Control values
	void InitInputDevices();
	InputHandler myInputHandler;

	// Texture values
	SDL_Texture* shipTex;
	SDL_Texture* asteroidTexSmall;
	SDL_Texture* asteroidTexMedium;
	SDL_Texture* shotTex;
	SDL_Texture* bombTex;
	TTF_Font* m_font;
	TTF_Font* m_fontHuge;

	void InitTextures();
	SDL_Texture* TextureFromPath(const char* path);

	// Main menu values
	GameSave myGameSave;
	MainMenu myMainMenu;
	PauseMenu myPauseMenu;

	void InitMenu();

	// Gameplay values
	Background gameBackground;
	std::list<GameObject*> gameObjectPtrs;

	void InitGameplay();

	inline static int score;
	inline static int life;
	inline static int bombCount;

	/// Values for wave spwan system
	float timeSinceLastAsteroidWave = 0;
	int asteroidWave = 1;

	/// Interaction values
	bool newClick = true;
	bool newPausePress = true;
	bool gameIsPaused = false;
	bool newBombIgnition = true;

	// UI values
	class UICounter* UIScore;
	class UICounter* UILives;
	class UICounter* UIBomb;
	class UICounter* UIFPS;
	class ShotMeter* shotMeter; // Alternative shot meter rendered below ship

	void InitUI();

	// deltaTime calculation
	Uint32 lastUpdateTime;

	// Performance values
	bool showUpdateTime = false;
	bool showRenderTime = false;
	bool showLoopTime = false;
	bool showFrameTime = false;
	bool showFPS = false;

	std::vector<float> FPSVector;

	//
	// Update
	//

	float CalculateDeltaTime();
	bool UpdateGameState();

	CollisionHandler myCollisionhandler;
};
