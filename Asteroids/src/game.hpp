#pragma once

#include <vector>
#include <list>
#include <stack>

#include "SDL_ttf.h"
#include "SDL_stdinc.h"

#include "menu/menu.hpp"
#include "saving/gamesave.hpp"
#include "states/gamestate.hpp"
#include "states/menustate.h"
#include "states/levelstate.h"
#include "states/pausestate.h"
#include "objects/background.hpp"
#include "physics/collisionhandler.h"
#include "audio/audioplayer.hpp"
#include "ui/UIelements.hpp"

#include "objects/gameobjects/asteroid.h"
#include "objects/gameobjects/bomb.h"
#include "objects/gameobjects/powerup.h"
#include "objects/gameobjects/ship.h"
#include "objects/gameobjects/shot.h"
#include "objects/gameobjects/follower.h"

#define PI 3.14159265359

// Gameplay parameters
constexpr int   STARTING_LIVES = 3;
// Spawn a bomb for every points
constexpr int   COLLECTABLE_SPAWN_ON_SCORE = 50;
// Time between asteroid waves
constexpr float ASTEROID_SPAWN_DELTATIME = 3.0f;
// Determines speed of newly spwaned asteroids multiplied by the current score
constexpr float ASTEROID_SPAWN_SPEED_MULTI = 0.3f;

class Game
{
public:
	friend MenuState;
	inline static MenuState menuState;
	friend LevelState;
	inline static LevelState levelState;
	friend PauseState;
	inline static PauseState pauseState;

	inline static std::vector<Ship> ships;
	inline static std::vector<Asteroid> asteroids;
	inline static std::vector<Shot> shots;
	inline static std::vector<Bomb> bombs;
	inline static std::vector<PowerUp> powerUps;
	inline static std::vector<Follower> followers;
	inline static std::vector<UICounter> UICounters;

	Game();
	~Game() {};
	void Init(const char* title, int xpos, int ypos, int m_width, int m_height);
	void HandleEvents();
	void Update();
	void Render();
	void Clean();

	void HandlePhysics();

	void PrintPerformanceInfo(Uint32 updateTime, Uint32 renderTime, Uint32 loopTime, Uint32 frameTime);
	bool GetIsRunning() const { return m_isRunning; }

	void ResetAllGameObjects();
	void SetGameplayStartValues();
	void SpawnAsteroidWave();

	static void IncreaseScore() { score++; }
	int GetScore() { return score; }
	static void DecreaseLife() { life--; }
	int GetLife() const { return life; }

	Vec2				GetLogicWindowDim() const { return Vec2((float)m_logicWidth, (float)m_logicHeight); }
	AudioPlayer&		GetAudioPlayer() { return m_audioPlayer; }
	MainMenu&			GetMainMenu() { return m_mainMenu; }
	PauseMenu&			GetPauseMenu() { return m_pauseMenu; }
	GameSave&			GetGameSave() { return m_gameSave; }
	CollisionHandler&	GetCollisionHandler() { return m_collisionhandler; }
	Background&			GetBackground() { return m_background; }
	
	std::vector<class GameObject*> GetGameObjectPtrs();

	void SetNewPausePress(bool value) { m_newPausePress = value; }
	bool GetNewPausePress() const { return m_newPausePress; }

	void AddTimeSinceLastWave(float time) { m_timeSinceLastAsteroidWave += time; }
	float GetTimeLastWave() const { return m_timeSinceLastAsteroidWave; }

	void ChangeState(GameState* newState) { m_gameState.emplace(newState); }
	void PushState(GameState* newTemporaryState) { m_gameState.push(newTemporaryState); }
	void PopState() { m_gameState.pop(); }
	void ExitGame() { SDL_Delay(200); m_isRunning = false; }
	void SpawnCollectable(const Vec2& position);

private:
	float m_deltaTime = 0.f;
	std::stack<GameState*, std::vector<GameState*>> m_gameState;

	inline static bool m_isRunning = true;
	// Game window values
	int m_logicWidth = 1280;
	int m_logicHeight = 720;
	SDL_Window* m_window = nullptr;
	SDL_Renderer* m_renderer = nullptr;
	void InitWindow(const char* title, int xpos, int ypos, int m_width, int m_height);

	// Control values
	void InitInputDevices();
	std::vector<SDL_GameController*> m_gameControllers;
	InputHandler m_inputHandler;

	// Sound values
	void InitSound();
	AudioPlayer m_audioPlayer;

	// Texture values
	TTF_Font* m_font = nullptr;
	TTF_Font* m_fontHuge = nullptr;

	void InitTextures();
	SDL_Texture* TextureFromPath(const char* path);

	// Main menu values
	GameSave m_gameSave;
	MainMenu m_mainMenu;
	PauseMenu m_pauseMenu;

	void InitMenu();

	// Gameplay values
	CollisionHandler m_collisionhandler;
	Background m_background;

	void InitGameplay();
	void SetTimeLastWave(float newValue) { m_timeSinceLastAsteroidWave = newValue; }

	inline static int score;
	inline static int life;
	inline static int bombCount;

	/// Values for wave spwan system
	float m_asteroidStartVelocity = 3.f;
	float m_timeSinceLastAsteroidWave = 0.f;
	int m_asteroidWave = 1;

	/// Interaction values
	bool m_newPausePress = true;

	void InitUI();

	// deltaTime calculation
	Uint32 m_lastUpdateTime = 0;

	// Performance values
	bool m_showUpdateTime = false;
	bool m_showRenderTime = false;
	bool m_showLoopTime = false;
	bool m_showFrameTime = false;
	bool m_showFPS = false;

	std::vector<float> m_FPSVector;
	float GetAverageFPS();

	// Update
	float CalculateDeltaTime();
};
