#include "game.hpp"

#include <stdexcept>
#include <numeric>
#include <iostream>
#include <math.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_gamecontroller.h"
#include "SDL_mixer.h"

#include "objects/shapes.hpp"
#include "objects/background.hpp"
#include "objects/helper.hpp"
#include "saving/gamesave.hpp"
#include "input/inputhandler.hpp"
#include "menu/menu.hpp"
#include "states/gamestate.hpp"
#include "vector2.hpp"

Game::Game()
{
	ships.reserve(1);
	asteroids.reserve(200);
	shots.reserve(30);
	bombs.reserve(10);
}

void Game::Init(const char* title, int xpos, int ypos, int m_width, int m_height)
{
	InitWindow(title, xpos, ypos, m_width, m_height);
	InitInputDevices();

	m_inputHandler = InputHandler();
	m_inputHandler.SetGameControllers(m_gameControllers);

	InitSound();
	InitTextures();
	InitMenu();
	InitGameplay();
	InitUI();
	// Priming game time
	m_lastUpdateTime = SDL_GetTicks();
}

void Game::InitWindow(const char* title, int xpos, int ypos, int m_width, int m_height)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		m_window = SDL_CreateWindow(title, xpos, ypos, m_width, m_height, SDL_WINDOW_RESIZABLE); // SDL_WINDOW_FULLSCREEN
		if (m_window)
		{
			std::cout << "Window created" << std::endl;
		}

		m_renderer = SDL_CreateRenderer(m_window, -1, 0); // For vsync -> SDL_RENDERER_PRESENTVSYNC
		if (m_renderer)
		{
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
			SDL_RenderSetLogicalSize(m_renderer, m_logicWidth, m_logicHeight);
			std::cout << "Renderer created!" << std::endl;
		}
		m_isRunning = true;
	}
	else
	{
		m_isRunning = false;
	}
}

void Game::InitInputDevices()
{
	int joystickCnt = SDL_NumJoysticks();
	if (joystickCnt > 0)
	{
		std::cout << joystickCnt << " joysticks were found." << std::endl;
	}
	for (int i = 0; i < joystickCnt; i++)
	{
		if (SDL_IsGameController(i))
		{
			std::cout << "Joystick " << i + 1 << " is supported by the game controller interface!" << std::endl;
			SDL_Joystick* joystick = SDL_JoystickOpen(i);
			std::cout << "The name of the joystick is : " << SDL_JoystickName(joystick) << std::endl;
		}
	}

	SDL_GameController* gamepad = nullptr;
	for (int i = 0; i < SDL_NumJoysticks(); ++i)
	{
		if (SDL_IsGameController(i))
		{
			gamepad = SDL_GameControllerOpen(i);
			if (gamepad)
			{
				std::cout << "Gamecontroller opened!" << std::endl;
				m_gameControllers.push_back(gamepad);
				break;
			}
			else
			{
				std::cout << "Could not open gamecontroller " << i << ": " << SDL_GetError() << std::endl;
			}
		}
	}
}

void Game::InitSound()
{
	int flags = MIX_INIT_OGG|MIX_INIT_MP3;
	if (Mix_Init(flags))
		std::cout << "Sound subsystem Initialised!..." << std::endl;
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == 0)
		std::cout << "SoundMixer opened!..." << std::endl;

	m_audioPlayer = AudioPlayer();
	GameObject::SetAudioPlayer(&m_audioPlayer);
}

void Game::InitTextures()
{
	if (IMG_Init(IMG_INIT_PNG))
		std::cout << "Image subsystem Initialised!..." << std::endl;
	else
		std::cout << IMG_GetError() << std::endl;
	if (TTF_Init() == 0)
		std::cout << "Font subsystem Initialised!..." << std::endl;
	else
		std::cout << TTF_GetError() << std::endl;

	Ship::SetTexture(TextureFromPath("./img/ship_thrustanimation.png"));
	Asteroid::SetTextureSmall(TextureFromPath("./img/asteroid_small1.png"));
	Asteroid::SetTextureMedium(TextureFromPath("./img/asteroid_medium1.png"));
	Shot::SetTexture(TextureFromPath("./img/shot.png"));
	Bomb::SetTexture(TextureFromPath("./img/bomb.png"));
	Follower::SetTexture(TextureFromPath("./img/follower.png"));

	m_font = TTF_OpenFont("./font/joystix_monospace.ttf", 20);
	m_fontHuge = TTF_OpenFont("./font/joystix_monospace.ttf", 120);
}

SDL_Texture* Game::TextureFromPath(const char* path)
{
	SDL_Surface* image = IMG_Load(path);
	if (image == NULL)
	{
		std::cout << IMG_GetError() << std::endl;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, image);
	SDL_FreeSurface(image);
	return texture;
}

void Game::InitMenu()
{
	m_gameSave = GameSave();
	m_audioPlayer.SetMasterVolume(m_gameSave.GetMasterVolume());

	m_mainMenu = MainMenu(m_font, m_fontHuge, m_renderer, m_logicWidth, m_logicHeight, this);
	m_mainMenu.CreateDefaultMainMenu();
	m_mainMenu.UpdateScore(m_gameSave.GetHighscore());

	m_pauseMenu = PauseMenu(m_font, m_fontHuge, m_renderer, m_logicWidth, m_logicHeight, this);
	m_pauseMenu.CreateDefaultPauseMenu();
}

void Game::InitGameplay()
{
	m_collisionhandler = CollisionHandler(this);

	m_background = Background(m_logicWidth, m_logicHeight, 2.0f, m_renderer);

	GameObject::SetRenderer(m_renderer);
	ChangeState(&menuState);
	m_gameState.top()->Enter(this);
}

void Game::InitUI()
{
	SDL_Color white = { 255, 255, 255, 255 };

	UICounters.push_back(
		UICounter("Score", m_font, white, m_renderer, 32, 16, 
			UICounterPosition::Left, [&]() { return this->GetScore(); }));
	UICounters.push_back(
		UICounter("Lives", m_font, white, m_renderer, 32, 16, 
			UICounterPosition::Right, [&]() { return this->GetLife(); }));
	UICounters.push_back(
		UICounter("Bombs", m_font, white, m_renderer, 32, 16, 
			UICounterPosition::Right, [&]() { return ships[0].GetCollectedBombsSize(); }));
	UICounters.push_back(
		UICounter("FPS", m_font, white, m_renderer, 32, 16,
			UICounterPosition::Right, [&]() { return (int)this->GetAverageFPS(); }));
}

void Game::HandleEvents()
{
	m_inputHandler.HandleInput(m_isRunning);

	GameState* prevGameState = m_gameState.top();
	m_gameState.top()->HandleEvents(m_inputHandler);
	if (prevGameState != m_gameState.top())
	{
		prevGameState->Exit();
		m_gameState.top()->Enter(this);
	}
}

void Game::Update()
{
	m_deltaTime = CalculateDeltaTime();

	m_gameState.top()->Update(m_deltaTime);
}

void Game::HandlePhysics()
{
	std::vector<class GameObject*>& allGameObjectPtrs = GetGameObjectPtrs();
	m_collisionhandler.CheckCollisions(allGameObjectPtrs);
}

std::vector<class GameObject*> Game::GetGameObjectPtrs()
{
	std::vector<GameObject*> allGameObjects;
	for (Ship& ship : ships)
	{
		allGameObjects.push_back(&ship);
	}
	for (Asteroid& asteroid : asteroids)
	{
		allGameObjects.push_back(&asteroid);
	}
	for (Shot& shot : shots)
	{
		allGameObjects.push_back(&shot);
	}
	for (Bomb& bomb : bombs)
	{
		allGameObjects.push_back(&bomb);
	}
	for (Follower& follower : followers)
	{
		allGameObjects.push_back(&follower);
	}
	return allGameObjects;
}

float Game::CalculateDeltaTime()
{
	Uint32 currentTime = SDL_GetTicks();
	float deltaTime = (currentTime - m_lastUpdateTime) / 1000.0f;
	m_lastUpdateTime = currentTime;
	return deltaTime;
}

void Game::Render()
{
	// Render are outside the playArea grey.
	// Only visible with aspect ratios unequal to 16/9.
	SDL_SetRenderDrawColor(m_renderer, 40, 40, 40, 255);
	SDL_RenderClear(m_renderer);

	// Render playArea black.
	SDL_Rect playArea = { 0, 0, m_logicWidth, m_logicHeight };
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(m_renderer, &playArea);

	m_gameState.top()->Render(m_renderer);

	SDL_RenderPresent(m_renderer);
}

void Game::Clean()
{
	SDL_DestroyWindow(m_window);
	SDL_DestroyRenderer(m_renderer);
	TTF_CloseFont(m_fontHuge);
	TTF_CloseFont(m_font);
	Mix_CloseAudio();
	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	std::cout << "Game Cleaned" << std::endl;
}

void Game::PrintPerformanceInfo(Uint32 updateTime, Uint32 renderTime, Uint32 loopTime, Uint32 frameTime)
{
	if (m_showUpdateTime)
		std::cout << "Update Time: " << updateTime << " ";
	if (m_showRenderTime)
		std::cout << "Render Time: " << renderTime << " ";
	if (m_showLoopTime)
		std::cout << "Frame Time: " << loopTime << " ";
	if (m_showFrameTime) // Loop time plus potential wait time
		std::cout << "Delayed Frame Time: " << frameTime << " ";
	if (m_showFPS)
		std::cout << "FPS: " << 1000 / frameTime << " ";
	if (m_showUpdateTime || m_showRenderTime || m_showLoopTime || m_showFrameTime || m_showFPS)
		std::cout << std::endl;
}

void Game::ResetAllGameObjects()
{
	GameObject::ResetId();
	ships.clear();
	asteroids.clear();
	shots.clear();
	bombs.clear();
	followers.clear();

	Vec2 midScreenPos = GetLogicWindowDim() / 2.f;
	Ship ship = Ship(midScreenPos, 50);
	ships.push_back(ship);

	int asteroidAmountSmall = 5;
	int asteroidAmountMedium = 5;
	std::vector<Asteroid> newAsteroids = InitAsteroids(asteroidAmountSmall, asteroidAmountMedium, GetLogicWindowDim());
	asteroids.insert(asteroids.end(), newAsteroids.begin(), newAsteroids.end());
}

void Game::SetGameplayStartValues()
{
	life = STARTING_LIVES;

	m_timeSinceLastAsteroidWave = 0.f;
	m_asteroidWave = 1;
	score = 0;
	bombCount = 0;
}

void Game::SpawnAsteroidWave()
{
	int size = Asteroid::GetSize(Asteroid::SizeType::Small);
	float colRadius = Asteroid::GetColRadius(size);
	Vec2 randomPos = GetFreeRandomPosition(GetLogicWindowDim(), colRadius, GetGameObjectPtrs());
	Vec2 randomVelocity = GetRandomVelocity(m_asteroidStartVelocity, m_asteroidStartVelocity + ASTEROID_SPAWN_SPEED_MULTI * score);

	Asteroid newAsteroid = Asteroid(randomPos, randomVelocity, Asteroid::SizeType::Small);
	asteroids.push_back(newAsteroid);

	Vec2 randomPos2 = GetFreeRandomPosition(GetLogicWindowDim(), colRadius, GetGameObjectPtrs());

	Follower follower(randomPos2, 30);
	followers.push_back(follower);

	if (m_asteroidWave % 3 == 0)
	{
		int size = Asteroid::GetSize(Asteroid::SizeType::Medium);
		float colRadius = Asteroid::GetColRadius(size);
		Vec2 randomPos = GetFreeRandomPosition(GetLogicWindowDim(), colRadius, GetGameObjectPtrs());
		Vec2 randomVelocity = GetRandomVelocity(m_asteroidStartVelocity, m_asteroidStartVelocity + ASTEROID_SPAWN_SPEED_MULTI * score);
		Asteroid newAsteroid = Asteroid(randomPos, randomVelocity, Asteroid::SizeType::Medium);
		asteroids.push_back(newAsteroid);
	}

	SetTimeLastWave(0.f);
	m_asteroidWave++;
}

float Game::GetAverageFPS()
{
	if (m_deltaTime > 0)
	{
		float FPS = 1 / m_deltaTime;
		if (m_FPSVector.size() >= 1000)
		{
			m_FPSVector.insert(m_FPSVector.begin(), FPS);
			m_FPSVector.pop_back();
		}
		else
		{
			m_FPSVector.insert(m_FPSVector.begin(), FPS);
		}
	}

	if (!m_FPSVector.empty())
	{
		float count = (float)(m_FPSVector.size());
		return std::accumulate(m_FPSVector.begin(), m_FPSVector.end(), 0.f) / count;
	}
	else
	{
		return 0.f;
	}
}