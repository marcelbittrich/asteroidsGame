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
#include "objects/initialization.hpp"
#include "ui/UIelements.hpp"
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

	myInputHandler = InputHandler();

	InitSound();
	InitTextures();
	InitMenu();
	InitGameplay();
	InitUI();
	// Priming game time
	lastUpdateTime = SDL_GetTicks();
}

void Game::InitWindow(const char* title, int xpos, int ypos, int m_width, int m_height)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		window = SDL_CreateWindow(title, xpos, ypos, m_width, m_height, SDL_WINDOW_RESIZABLE); // SDL_WINDOW_FULLSCREEN
		if (window)
		{
			std::cout << "Window created" << std::endl;
			windowWidth = 1280;
			windowHeight = 720;
		}

		renderer = SDL_CreateRenderer(window, -1, 0); // For vsync -> SDL_RENDERER_PRESENTVSYNC
		if (renderer)
		{
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
			SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);
			std::cout << "Renderer created!" << std::endl;
		}
		isRunning = true;
	}
	else
	{
		isRunning = false;
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

	myAudioPlayer = AudioPlayer();
	GameObject::SetAudioPlayer(&myAudioPlayer);
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
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_FreeSurface(image);
	return texture;
}

void Game::InitMenu()
{
	myGameSave = GameSave();
	myAudioPlayer.SetMasterVolume(myGameSave.GetMasterVolume());

	myMainMenu = MainMenu(m_font, m_fontHuge, renderer, windowWidth, windowHeight, this);
	myMainMenu.CreateDefaultMainMenu();
	myMainMenu.UpdateScore(myGameSave.GetHighscore());

	myPauseMenu = PauseMenu(m_font, m_fontHuge, renderer, windowWidth, windowHeight, this);
	myPauseMenu.CreateDefaultPauseMenu();
}

void Game::InitGameplay()
{
	myCollisionhandler = CollisionHandler(this);

	gameBackground = Background(windowWidth, windowHeight, 2.0f, renderer);

	GameObject::SetRenderer(renderer);
	ChangeState(&menuState);
	gameState.top()->Enter(this);
}

void Game::InitUI()
{
	SDL_Color white = { 255, 255, 255, 255 };
	UICounter("Score", m_font, white, renderer, 32, 16, UICounterPosition::Left, [&]() { return this->GetScore(); });
	UICounter("Lives", m_font, white, renderer, 32, 16, UICounterPosition::Right, [&]() { return this->GetLife(); });
	UICounter("Bombs", m_font, white, renderer, 32, 16, UICounterPosition::Right, [&]() { return ships[0].GetCollectedBombsSize(); });
	UICounter("FPS", m_font, white, renderer, 32, 16, UICounterPosition::Right, [&]() { return (int)this->GetAverageFPS(); });
}

void Game::HandleEvents()
{
	myInputHandler.HandleInput(isRunning);

	GameState* prevGameState = gameState.top();
	gameState.top()->HandleEvents(myInputHandler);
	if (prevGameState != gameState.top())
	{
		prevGameState->Exit();
		gameState.top()->Enter(this);
	}
}

void Game::Update()
{
	m_deltaTime = CalculateDeltaTime();

	gameState.top()->Update(m_deltaTime);
}

void Game::HandlePhysics()
{
	std::vector<class GameObject*>& allGameObjectPtrs = GetGameObjectPtrs();
	myCollisionhandler.CheckCollisions(allGameObjectPtrs);
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
	float deltaTime = (currentTime - lastUpdateTime) / 1000.0f;
	lastUpdateTime = currentTime;
	return deltaTime;
}

void Game::Render()
{
	// Render are outside the playArea grey.
	// Only visible with aspect ratios unequal to 16/9.
	SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
	SDL_RenderClear(renderer);

	// Render playArea black.
	SDL_Rect playArea = { 0, 0, windowWidth, windowHeight };
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &playArea);

	gameState.top()->Render(renderer);

	SDL_RenderPresent(renderer);
}

void Game::Clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
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
	if (showUpdateTime)
		std::cout << "Update Time: " << updateTime << " ";
	if (showRenderTime)
		std::cout << "Render Time: " << renderTime << " ";
	if (showLoopTime)
		std::cout << "Frame Time: " << loopTime << " ";
	if (showFrameTime) // Loop time plus potential wait time
		std::cout << "Delayed Frame Time: " << frameTime << " ";
	if (showFPS)
		std::cout << "FPS: " << 1000 / frameTime << " ";
	if (showUpdateTime || showRenderTime || showLoopTime || showFrameTime || showFPS)
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

	Vec2 midScreenPos = GetWindowDim() / 2.f;
	Ship ship = Ship(midScreenPos, 50, shipTex);
	ships.push_back(ship);

	int asteroidAmountSmall = 5;
	int asteroidAmountMedium = 5;
	std::vector<Asteroid> newAsteroids = InitAsteroids(asteroidAmountSmall, asteroidAmountMedium, GetWindowDim());
	asteroids.insert(asteroids.end(), newAsteroids.begin(), newAsteroids.end());
}

void Game::SetGameplayStartValues()
{
	life = STARTING_LIVES;

	timeSinceLastAsteroidWave = 0.f;
	asteroidWave = 1;
	score = 0;
	bombCount = 0;
}

void Game::SpawnAsteroidWave()
{
	int size = Asteroid::GetSize(Asteroid::SizeType::Small);
	float colRadius = Asteroid::GetColRadius(size);
	Vec2 randomPos = GetFreeRandomPosition(GetWindowDim(), colRadius, GetGameObjectPtrs());
	Vec2 randomVelocity = GetRandomVelocity(0, ASTEROID_SPAWN_SPEED_MULTI * score);

	Asteroid newAsteroid = Asteroid(randomPos, randomVelocity, Asteroid::SizeType::Small);
	asteroids.push_back(newAsteroid);

	Vec2 randomPos2 = GetFreeRandomPosition(GetWindowDim(), colRadius, GetGameObjectPtrs());
	Follower follower = Follower(randomPos2, 30);
	followers.push_back(follower);

	if (asteroidWave % 3 == 0)
	{
		int size = Asteroid::GetSize(Asteroid::SizeType::Medium);
		float colRadius = Asteroid::GetColRadius(size);
		Vec2 randomPos = GetFreeRandomPosition(GetWindowDim(), colRadius, GetGameObjectPtrs());
		Vec2 randomVelocity = GetRandomVelocity(0, ASTEROID_SPAWN_SPEED_MULTI * score);
		Asteroid newAsteroid = Asteroid(randomPos, randomVelocity, Asteroid::SizeType::Medium);
		asteroids.push_back(newAsteroid);
	}

	SetTimeLastWave(0.f);
	asteroidWave++;
}

float Game::GetAverageFPS()
{
	if (m_deltaTime > 0)
	{
		float FPS = 1 / m_deltaTime;
		if (FPSVector.size() >= 1000)
		{
			FPSVector.insert(FPSVector.begin(), FPS);
			FPSVector.pop_back();
		}
		else
		{
			FPSVector.insert(FPSVector.begin(), FPS);
		}
	}

	if (!FPSVector.empty())
	{
		float count = (float)(FPSVector.size());
		return std::accumulate(FPSVector.begin(), FPSVector.end(), 0.f) / count;
	}
	else
	{
		return 0.f;
	}
}