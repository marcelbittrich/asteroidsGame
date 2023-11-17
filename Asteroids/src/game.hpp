#pragma once

#include <vector>
#include <list>

#include "SDL_ttf.h"
#include "SDL_stdinc.h"

//#include "gameobjects.hpp"
#include "menu/menu.hpp"
#include "saving/gamesave.hpp"
#include "states/gamestates.hpp"
#include "objects/background.hpp"
#include "physics/collisionhandler.h"
#include "audio/audioplayer.hpp"
//#include "UIelements.hpp"

#define PI 3.14159265359

// Gameplay parameters
const int STARTING_LIVES = 3;
// Spawn a bomb for every points
const int BOMB_SPAWN_ON_SCORE = 50;
// Time between asteroid waves
const float ASTEROID_SPAWN_DELTATIME = 3.0f;
// Determines speed of newly spwaned asteroids multiplied by the current score
const float ASTEROID_SPAWN_SPEED_MULTI = 0.03f;

class Game
{
public:
	Game() {};
	~Game() {};
	void Init(const char* title, int xpos, int ypos, int m_width, int m_height);
	void HandleEvents();
	void Update();
	void Render();
	void Clean();

	void PrintPerformanceInfo(Uint32 updateTime, Uint32 renderTime, Uint32 loopTime, Uint32 frameTime);
	bool GetIsRunning() const { return isRunning; }

	void ResetAllGameObjects();
	void InitGameplayValues();
	void SpawnAsteroidWave();

	static void IncreaseScore() { score++; }
	int GetScore() { return score; }
	static void DecreseLife() { life--; }
	int GetLife() const { return life; }

	SDL_Point			GetWindowDim() const { return { windowWidth, windowHeight }; }
	class GameState*    GetState() const { return gameState; }
	AudioPlayer&		GetAudioPlayer() { return myAudioPlayer; }
	MainMenu&			GetMainMenu() { return myMainMenu; }
	PauseMenu&			GetPauseMenu() { return myPauseMenu; }
	GameSave&			GetGameSave() { return myGameSave; }
	CollisionHandler&	GetCollisionHandler() { return myCollisionhandler; }
	Background&			GetBackground() { return gameBackground; }
	std::list<class GameObject*>& GetGameObjectPtrs() { return gameObjectPtrs; }

	void SetNewPausePress(bool value) { newPausePress = value; }
	bool GetNewPausePress() { return newPausePress; }
	void SetTimeLastWave(float newValue) { timeSinceLastAsteroidWave = newValue; }
	void AddTimeLastWave(float time) { timeSinceLastAsteroidWave += time; }
	float GetTimeLastWave() const { return timeSinceLastAsteroidWave; }
	float GetAverageFPS();

	void ChangeState(GameState* newState) { gameState = newState; }
	void ExitGame() { SDL_Delay(200); isRunning = false; }

	friend MenuState;
	inline static MenuState menuState;
	friend LevelState;
	inline static LevelState levelState;
	friend PauseState;
	inline static PauseState pauseState;

private:
	float m_deltaTime = 0.f;
	GameState* gameState = &menuState;

	inline static bool isRunning = true;
	// Game window values
	int windowWidth;
	int windowHeight;
	SDL_Window* window;
	SDL_Renderer* renderer;
	void InitWindow(const char* title, int xpos, int ypos, int m_width, int m_height);

	// Control values
	void InitInputDevices();
	InputHandler myInputHandler;

	// Sound values
	void InitSound();
	AudioPlayer myAudioPlayer;

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
	CollisionHandler myCollisionhandler;
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

	// Update
	float CalculateDeltaTime();
};
