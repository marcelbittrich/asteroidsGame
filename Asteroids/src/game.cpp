#include <stdexcept>
#include <numeric>
#include <iostream>
#include <math.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_gamecontroller.h"

#include "shapes.hpp"
#include "background.hpp"
#include "initialization.hpp"
#include "UIelements.hpp"
#include "gamesave.hpp"
#include "inputhandler.hpp"
#include "game.hpp"
#include "menu.hpp"
#include "gamestate.hpp"
#include "vector2.hpp"

Game::Game()
{
}
Game::~Game()
{
}

void Game::Init(const char* title, int xpos, int ypos, int m_width, int m_height, bool fullscreen)
{
	InitWindow(title, xpos, ypos, m_width, m_height, fullscreen);
	InitInputDevices();

	myInputHandler = InputHandler();

	InitTextures();
	InitMenu();
	InitGameplay();
	InitUI();

	// priming game time
	lastUpdateTime = SDL_GetTicks();
}

void Game::InitWindow(const char* title, int xpos, int ypos, int m_width, int m_height, bool fullscreen)
{
	int flags = 0;
	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{
		if (IMG_Init(IMG_INIT_PNG))
			std::cout << "Iamge subsystem Initialised!..." << std::endl;
		if (TTF_Init() == 0)
			std::cout << "Font subsystem Initialised!..." << std::endl;
		else
			std::cout << TTF_GetError() << std::endl;

		window = SDL_CreateWindow(title, xpos, ypos, m_width, m_height, SDL_WINDOW_RESIZABLE);
		if (window)
		{
			std::cout << "Window created" << std::endl;
			windowWidth = 1280;
			windowHeight = 720;
		}

		renderer = SDL_CreateRenderer(window, -1, 0); // SDL_RENDERER_PRESENTVSYNC
		if (renderer)
		{
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
			SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
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
	int joystticksCnt = SDL_NumJoysticks();
	if (joystticksCnt > 0)
	{
		std::cout << joystticksCnt << " joysticks were found." << std::endl;
	}
	for (int i = 0; i < joystticksCnt; i++)
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

void Game::InitTextures()
{
	shipTex = TextureFromPath("./img/ship_thrustanimation.png");
	Ship::SetTexture(shipTex);
	asteroidTexSmall = TextureFromPath("./img/asteroid_small1.png");
	Asteroid::SetTextureSmall(asteroidTexSmall);
	asteroidTexMedium = TextureFromPath("./img/asteroid_medium1.png");
	Asteroid::SetTextureMedium(asteroidTexMedium);
	shotTex = TextureFromPath("./img/shot.png");
	Shot::SetTexture(shotTex);
	bombTex = TextureFromPath("./img/bomb.png");
	Bomb::SetTexture(bombTex);

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
	myMainMenu = MainMenu(m_font, m_fontHuge, renderer, windowWidth, windowHeight);
	myMainMenu.CreateDefaultMainMenu();
	myMainMenu.UpdateScore(myGameSave.GetHighscore());
}

void Game::InitGameplay()
{
	gameBackground = Background(windowWidth, windowHeight, 2.0f);

	GameObject::SetRenderer(renderer);
	Ship(Vec2(windowWidth / 2.f, windowHeight / 2.f), 50, shipTex);
	InitAsteroids(windowWidth, windowHeight);

	score = 0;
	life = STARTING_LIVES;
	bombCount = STARTING_BOMB_COUNT;

	myCollisionhandler = CollisionHandler();
}

void Game::InitUI()
{
	//shotMeter = new ShotMeter(ship, 0, 25, 40, 6);

	SDL_Color white = { 255, 255, 255, 255 };
	UIScore = new UICounter("Score", m_font, white, windowWidth, windowHeight, 32, 16, UICounterPosition::Left, true);
	UILives = new UICounter("Lives", m_font, white, windowWidth, windowHeight, 32, 16, UICounterPosition::Right, true);
	UIBomb = new UICounter("Bombs", m_font, white, windowWidth, windowHeight, 32, 16, UICounterPosition::Right, true);
	UIFPS = new UICounter("FPS", m_font, white, windowWidth, windowHeight, 32, 16, UICounterPosition::Right, true);
}

void Game::HandleEvents()
{
	myInputHandler.HandleInput(isRunning);

	myCollisionhandler.CheckCollisions(gameObjectPtrs);

	//
	// DESTROY ALL DEAD STUFF
	//

	gameObjectPtrs.clear();

	for (auto shipIt = Ship::ships.begin(); shipIt != Ship::ships.end(); )
	{
		if (shipIt->GetIsDead())
		{
			shipIt = Ship::ships.erase(shipIt);
		}
		else
		{
			gameObjectPtrs.push_back(&*shipIt);
			shipIt++;
		}
	}

	for (auto asteroidIt = Asteroid::asteroids.begin(); asteroidIt != Asteroid::asteroids.end(); )
	{
		if (asteroidIt->GetIsDead())
		{
			asteroidIt = Asteroid::asteroids.erase(asteroidIt);
		}
		else
		{
			gameObjectPtrs.push_back(&*asteroidIt);
			asteroidIt++;
		}
	}

	// Shots
	for (auto shotIt = Shot::shots.begin(); shotIt != Shot::shots.end(); )
	{
		if (shotIt->GetIsDead())
		{
			shotIt = Shot::shots.erase(shotIt);
		}
		else
		{
			gameObjectPtrs.push_back(&*shotIt);
			shotIt++;
		}
	}

	// Bombs
	for (auto bombIt = Bomb::bombs.begin(); bombIt != Bomb::bombs.end(); )
	{
		bool IsDead = bombIt->GetIsDead();

		if (IsDead)
		{
			bombIt = Bomb::bombs.erase(bombIt);
		}
		else
		{
			gameObjectPtrs.push_back(&*bombIt);
			bombIt++;
		}
	}
}

void Game::Update()
{
	float deltaTime = CalculateDeltaTime();

	bool keepUpdating = UpdateGameState();
	if (!keepUpdating)
		return;

	bool isLeftClicking = (myInputHandler.GetControlBools()).isLeftClicking;

	// Debug Bomb Spawn on Click
	if (isLeftClicking && newClick)
	{
		newClick = false;
		int mouseXPos, mouseYPos;
		SDL_GetMouseState(&mouseXPos, &mouseYPos);
		std::cout << "Left Click at: " << mouseXPos << " " << mouseYPos << std::endl;
		// Bomb(mouseXPos, mouseYPos, getRandomVelocity(0.0f, 0.5f));
	}
	else if (!isLeftClicking)
	{
		newClick = true;
	}

	for (Ship& ship : Ship::ships)
	{
		ship.Update(myInputHandler, windowWidth, windowHeight, deltaTime);
	}

	for (Asteroid& asteroid : Asteroid::asteroids)
	{
		asteroid.Update(windowWidth, windowHeight, deltaTime);
	}

	// Spawn New Asteroids
	timeSinceLastAsteroidWave += deltaTime;
	if (timeSinceLastAsteroidWave >= ASTEROID_SPAWN_DELTATIME)
	{
		//std::cout << "Spawn small asteroid" << std::endl;
		Vec2 randomPos1 = GetRandomPosition(windowWidth, windowHeight, Asteroid::GetColRadius(Asteroid::GetSize(Asteroid::SizeType::Small)), gameObjectPtrs);
		Vec2 randomVelocity1 = GetRandomVelocity(0, ASTEROID_SPAWN_SPEED_MULTI * score);
		spawnAsteroid(randomPos1.x, randomPos1.y, randomVelocity1, Asteroid::SizeType::Small, gameObjectPtrs);

		if (asteroidWave % 3 == 0)
		{
			//std::cout << "Spawn large asteroid" << std::endl;
			Vec2 randomPos2 = GetRandomPosition(windowWidth, windowHeight, Asteroid::GetColRadius(Asteroid::GetSize(Asteroid::SizeType::Medium)), gameObjectPtrs);
			Vec2 randomVelocity2 = GetRandomVelocity(0, ASTEROID_SPAWN_SPEED_MULTI * score);
			spawnAsteroid(randomPos2.x, randomPos2.y, randomVelocity2, Asteroid::SizeType::Medium, gameObjectPtrs);
		}
		timeSinceLastAsteroidWave = 0;
		asteroidWave++;
	}

	// Alternative Shot Meter - not used
	// shotMeter->update(ship->getShotCounter(), ship->getMaxShotCounter(), ship);

	// Update Shots
	for (Shot& Shot : Shot::shots)
	{
		Shot.Update(windowWidth, windowHeight, deltaTime);
	}

	// Update Bombs
	for (Bomb& bomb : Bomb::bombs)
	{
		bomb.Update(windowWidth, windowHeight, deltaTime);
	}

	gameBackground.Update(gameObjectPtrs, deltaTime);

	if (deltaTime > 0)
	{
		float FPS = 1 / deltaTime;
		if (FPSVector.size() >= 10)
		{
			FPSVector.insert(FPSVector.begin(), FPS);
			FPSVector.pop_back();
		}
		else
		{
			FPSVector.insert(FPSVector.begin(), FPS);
		}
	}

	float averageFPS;
	if (!FPSVector.empty())
	{
		int count = (int)(FPSVector.size());
		averageFPS = std::reduce(FPSVector.begin(), FPSVector.end()) / (float)count;
	}
	else
	{
		averageFPS = 0;
	}

	UIScore->Update(score, renderer);
	UIFPS->Update((int)averageFPS, renderer);
	UILives->Update(life - 1, renderer);
	UIBomb->Update(Ship::ships[0].GetCollectedBombsSize(), renderer);
}

float Game::CalculateDeltaTime()
{
	Uint32 currentTime = SDL_GetTicks();
	float deltaTime = (currentTime - lastUpdateTime) / 1000.0f;
	lastUpdateTime = currentTime;
	return deltaTime;
}

bool Game::UpdateGameState()
{
	if (gameState == GameState::IN_MENU)
	{
		myMainMenu.Update(myInputHandler);
	}

	// Check if player just died and present menu with score
	if (life == 0 && gameState == GameState::IN_GAME)
	{
		gameState = GameState::IN_MENU;
		myMainMenu.ChangeState(MainMenu::State::GameOver);
		myMainMenu.UpdateScore(score);

		int oldHighscore = myGameSave.GetHighscore();
		if (score > oldHighscore)
		{
			myGameSave.SetHighscore(score);
			myGameSave.WriteFile();
		}
	}

	bool pausePressed = myInputHandler.GetControlBools().pausePressed;
	if (pausePressed && gameState == GameState::IN_GAME && newPausePress)
	{
		newPausePress = false;
		gameState = GameState::PAUSE;
	}
	else if (pausePressed && gameState == GameState::PAUSE && newPausePress)
	{
		newPausePress = false;
		gameState = GameState::IN_GAME;
	}
	else if (!pausePressed)
	{
		newPausePress = true;
	}

	return gameState == GameState::IN_GAME;
}

void Game::Render()
{
	if (gameState == GameState::IN_MENU || gameState == GameState::RESET)
	{
		myMainMenu.Render();
		return;
	}

	SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
	SDL_RenderClear(renderer);
	SDL_Rect playArea = { 0, 0, windowWidth, windowHeight };
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &playArea);

	gameBackground.Render(renderer);

	for (auto objectIt = gameObjectPtrs.begin(); objectIt != gameObjectPtrs.end(); objectIt++)
	{
		(*objectIt)->Render();
	}

	UIScore->Render(renderer);
	UILives->Render(renderer);
	UIBomb->Render(renderer);
	UIFPS->Render(renderer);

	// ShotMeter
	// shotMeter.render(renderer, ship.canShoot);

	if (life == 0)
	{
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
		SDL_RenderFillRect(renderer, &playArea);
	}

	SDL_RenderPresent(renderer);
}

void Game::Clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_CloseFont(m_fontHuge);
	TTF_CloseFont(m_font);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	std::cout << "Game Cleaned" << std::endl;
}

void Game::Reset()
{
	GameObject::ResetId();
	Ship::ships.clear();
	Asteroid::asteroids.clear();
	Shot::shots.clear();
	Bomb::bombs.clear();
	gameObjectPtrs.clear();

	timeSinceLastAsteroidWave = 0;
	asteroidWave = 1;

	score = 0;
	life = 3;
	bombCount = 0;

	Ship(Vec2(windowWidth / 2.f, windowHeight / 2.f), 50, shipTex);

	InitAsteroids(windowWidth, windowHeight);

	lastUpdateTime = SDL_GetTicks();

	gameState = GameState::IN_GAME;
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