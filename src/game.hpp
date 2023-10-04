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

#define PI 3.14159265359

class Game
{
public:
	Game();
	~Game();
	void Init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	void HandleEvents();
	void Update();
	void Render();
	void Clean();
	void Reset();
	GameState gameState = GameState::STATE_IN_MENU;

	void PrintPerformanceInfo(Uint32 updateTime, Uint32 renderTime, Uint32 loopTime, Uint32 frameTime);

	std::list<Bomb> Bombs;

	bool GetIsRunning() const { return isRunning; }

private:
	bool isRunning;
	// Game window values
	int windowWidth;
	int windowHeight;
	SDL_Window* window;
	SDL_Renderer* renderer;
	void InitWindow(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

	// Control values
	void InitInputDevices();
	InputHandler MyInputHandler;

	// Texture values
	SDL_Texture* shipTex;
	SDL_Texture* asteroidTexSmall;
	SDL_Texture* asteroidTexMedium;
	SDL_Texture* shotTex;
	SDL_Texture* bombTex;
	TTF_Font* font;
	TTF_Font* fontHuge;

	void initTextures();
	SDL_Texture* createTextureFromPath(const const char* path);

	// Main menu values
	GameSave myGameSave;
	GameMenu myGameMenu;

	void initMenu();

	// Gameplay values
	Ship ship;
	Background gameBackground;
	std::list<GameObject> gameObjects;
	std::list<GameObject*> gameObjectPtrs;

	void initGameplay();

	int score;
	int life;
	int bombCount;

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

	void initUI();

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

	float calculateDeltaTime();
	bool updateGameState();
};
