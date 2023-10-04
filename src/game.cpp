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

#include <filesystem>

namespace fs = std::filesystem;

// Gameplay parameters
const int STARTING_LIVES = 3;
const int STARTING_BOMB_COUNT = 0;
// One in ... for dropping a bomb when destroing astroids.
const int BOMB_SPAWN_ON_SCORE = 50;
const float ASTEROID_SPAWN_DELTATIME = 3.0f;
const float ASTEROID_SPAWN_SPEED_MULTI = 0.03f;

Game::Game()
{
}
Game::~Game()
{
}

void Game::Init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen)
{
    InitWindow(title, xpos, ypos, width, height, fullscreen);
    InitInputDevices();

    MyInputHandler = InputHandler();

    initTextures();
    initMenu();
    initGameplay();
    initUI();

    // priming game time
    lastUpdateTime = SDL_GetTicks();
}

#pragma region init functions

void Game::InitWindow(const char *title, int xpos, int ypos, int width, int height, bool fullscreen)
{
    int flags = 0;
    if (fullscreen)
    {
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        IMG_Init(IMG_INIT_PNG);
        std::cout << "Subsystem Initialised!..." << std::endl;
        if (TTF_Init())
            std::cout << "Font System Initialised!...";

        window = SDL_CreateWindow(title, xpos, ypos, width, height, SDL_WINDOW_RESIZABLE);
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
    printf("%i joysticks were found.\n", SDL_NumJoysticks());
    for (int i = 1; i <= SDL_NumJoysticks(); ++i)
    {
        if (SDL_IsGameController(i))
        {
            printf("Joystick %i is supported by the game controller interface!\n", i);
        }
    }
    printf("The names of the joysticks are:\n");
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        SDL_Joystick *joystick = SDL_JoystickOpen(i);
        printf("%s\n", SDL_JoystickName(joystick));
    }

    SDL_GameController *gamepad = nullptr;
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
                fprintf(stderr, "Could not open gamecontroller %i: %s\n", i, SDL_GetError());
            }
        }
    }
}

void Game::initTextures()
{
    std::cout << "Current path is " << fs::current_path() << '\n'; // (1)
    fs::current_path(fs::temp_directory_path()); // (3)
    std::cout << "Current path is " << fs::current_path() << '\n';

    shipTex = createTextureFromPath("D:/Dokumente/GameDev/Game/img/ship_thrustanimation.png");
    Ship::setTexture(shipTex);
    asteroidTexSmall = createTextureFromPath("D:/Dokumente/GameDev/Game/img/asteroid_small1.png");
    Asteroid::setTextureSmall(asteroidTexSmall);
    asteroidTexMedium = createTextureFromPath("D:/Dokumente/GameDev/Game/img/asteroid_medium1.png");
    Asteroid::setTextureMedium(asteroidTexMedium);
    shotTex = createTextureFromPath("D:/Dokumente/GameDev/Game/img/shot.png");
    Shot::setTexture(shotTex);
    bombTex = createTextureFromPath("D:/Dokumente/GameDev/Game/img/bomb.png");
    Bomb::setTexture(bombTex);

    font = TTF_OpenFont("D:/Dokumente/GameDev/Game/font/joystix_monospace.ttf", 20);
    fontHuge = TTF_OpenFont("D:/Dokumente/GameDev/Game/font/joystix_monospace.ttf", 120);
}

SDL_Texture* Game::createTextureFromPath(const char* path)
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

void Game::initMenu()
{
    myGameSave = GameSave();
    myGameMenu = GameMenu(font, fontHuge, renderer, windowWidth, windowHeight);
    myGameMenu.setHighscore(myGameSave.getHighscore());
}

void Game::initGameplay()
{
    gameBackground = Background(windowWidth, windowHeight);

    ship = Ship(windowWidth / 2, windowHeight / 2, 50, shipTex);
    gameObjects.push_back(ship);
    initAsteroids(ship, windowWidth, windowHeight);

    score = 0;
    life = STARTING_LIVES;
    bombCount = STARTING_BOMB_COUNT;
}

void Game::initUI()
{
    shotMeter = new ShotMeter(ship, 0, 25, 40, 6);

    SDL_Color white = {255, 255, 255, 255};
    UIScore = new UICounter("Score", font, white, windowWidth, windowHeight, 32, 16, UICounterPosition::Left, true);
    UILives = new UICounter("Lives", font, white, windowWidth, windowHeight, 32, 16, UICounterPosition::Right, true);
    UIBomb = new UICounter("Bombs", font, white, windowWidth, windowHeight, 32, 16, UICounterPosition::Right, true);
    UIFPS = new UICounter("FPS", font, white, windowWidth, windowHeight, 32, 16, UICounterPosition::Right, true);
}

#pragma endregion

void Game::HandleEvents()
{
    MyInputHandler.handleInput(isRunning);


    // Collision Asteroid Asteroid
    for (Asteroid& asteroid : Asteroid::asteroids)
    {
        if (doesCollide(ship, asteroid))
        {
            if (--life == 0)
            {
                break;
            }
            ship.respawn(renderer);
            asteroidWave = 1;
        }

        for (Asteroid& asteroid2 : Asteroid::asteroids)
        {
            // std::cout << "Kombination: " << i << ", " << j << std::endl;
            if (&asteroid != &asteroid2)
            {
                asteroidsCollide(asteroid, asteroid2);
            }
        }
    }

    // Collision Ship Asteroids
    for (Bomb& bomb : Bombs)
    {
        if (!bomb.isExploding && doesCollide(ship, bomb))
        {
            ship.collectBomb(&bomb);
        }
    }

    // Collisions Bomb Asteroid
    for (Bomb& bomb : Bombs)
    {
        if (bomb.isExploding)
        {
            for (auto asteroidIt = Asteroid::asteroids.begin(); asteroidIt != Asteroid::asteroids.end(); )
            {
                // std::cout << "Bomb colRadius: " << bomb->colRadius << std::endl;
                if (doesCollide(bomb, *asteroidIt))
                {
                    asteroidIt = Asteroid::asteroids.erase(asteroidIt);
                }
                else
                {
                    asteroidIt++;
                }

            }
        }
    }

    //
    // DESTROY ALL DEAD STUFF
    //

    // Shots
    for (auto shotIt = Shot::shots.begin(); shotIt != Shot::shots.end(); )
    {
        if (shotIt->getIsDead())
        {
            shotIt = Shot::shots.erase(shotIt);
        }
        else
        {
            shotIt++;
        }
    }

    // Bombs
    for (auto bombIterator = Bombs.begin(); bombIterator != Bombs.end();)
    {
        Bomb bomb = *bombIterator;
        if (bomb.isDead)
        {
            bombIterator = Bombs.erase(bombIterator);
        }
        else
        {
            bombIterator++;
        }
    }
}

void Game::Update()
{
    float deltaTime = calculateDeltaTime();

    bool keepUpdating = updateGameState();
    if (!keepUpdating)
        return;

    bool isLeftClicking = (MyInputHandler.getControlBools()).isLeftClicking;

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


    ship.update(MyInputHandler, windowWidth, windowHeight, deltaTime);

    for (Asteroid& asteroid : Asteroid::asteroids)
    {
        asteroid.update(windowWidth, windowHeight, deltaTime);
    }

    // Spawn New Asteroids
    timeSinceLastAsteroidWave += deltaTime;
    if (timeSinceLastAsteroidWave >= ASTEROID_SPAWN_DELTATIME)
    {
        std::cout << "Spawn small ";
        SDL_Point randomPos1 = getRandomPosition(windowWidth, windowHeight, Asteroid::getColRadius(Asteroid::getSize(AsteroidSizeType::Small)), gameObjects);
        SDL_FPoint randomVelocity1 = getRandomVelocity(0, ASTEROID_SPAWN_SPEED_MULTI * score);
        spawnAsteroid(randomPos1.x, randomPos1.y, randomVelocity1, AsteroidSizeType::Small, gameObjects);

        if (asteroidWave % 3 == 0)
        {
            std::cout << "and large ";
            SDL_Point randomPos2 = getRandomPosition(windowWidth, windowHeight, Asteroid::getColRadius(Asteroid::getSize(AsteroidSizeType::Medium)), gameObjects);
            SDL_FPoint randomVelocity2 = getRandomVelocity(0, ASTEROID_SPAWN_SPEED_MULTI * score);
            spawnAsteroid(randomPos2.x, randomPos2.y, randomVelocity2, AsteroidSizeType::Medium, gameObjects);
        }
        std::cout << "asteroid" << std::endl;
        timeSinceLastAsteroidWave = 0;
        asteroidWave++;
    }

    // Alternative Shot Meter - not used
    // shotMeter->update(ship->getShotCounter(), ship->getMaxShotCounter(), ship);

    // Update Shots
    for (Shot& Shot : Shot::shots)
    {
        Shot.update(windowWidth, windowHeight, deltaTime);
    }

    auto it = Shot::shots.begin();
    while (it != Shot::shots.end())
    {
        bool hit = false;

        auto asteroidIt = Asteroid::asteroids.begin();
        while (asteroidIt != Asteroid::asteroids.end())
        {
            if (doesCollide(*it, *asteroidIt))
            {
                it = Shot::shots.erase(it);

                hit = true;

                if (asteroidIt->sizeType == AsteroidSizeType::Small)
                {
                    if (score % BOMB_SPAWN_ON_SCORE == 0)
                    {
                        SDL_FPoint asteroidMidPos = asteroidIt->getMidPos();
                        Bomb bomb = Bomb((int)asteroidMidPos.x, (int)asteroidMidPos.y, getRandomVelocity(0.0f, 0.5f));
                        Bombs.push_back(bomb);
                    }

                    asteroidIt = Asteroid::asteroids.erase(asteroidIt);
                    score++;
                    break;
                }
                else if (asteroidIt->sizeType == AsteroidSizeType::Medium)
                {
                    handleDestruction(*asteroidIt);
                    asteroidIt = Asteroid::asteroids.erase(asteroidIt);
                    break;
                }

                break;
            }
            if (!hit)
                asteroidIt++;
        }

        if (!hit)
            it++;
    }

    // Update Bombs
    for (Bomb& bomb : Bombs)
    {
        bomb.update(windowWidth, windowHeight, deltaTime, &ship);
    }


    // Fill colObjects vector
    gameObjects.clear();
    gameObjects.push_back(ship);
    gameObjects.insert(gameObjects.end(), Shot::shots.begin(), Shot::shots.end());
    gameObjects.insert(gameObjects.end(), Asteroid::asteroids.begin(), Asteroid::asteroids.end());
    gameObjects.insert(gameObjects.end(), Bombs.begin(), Bombs.end());

    gameBackground.Update(gameObjects, deltaTime);

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

    UIScore->update(score, renderer);
    UIFPS->update((int)averageFPS, renderer);
    UILives->update(life - 1, renderer);
    UIBomb->update(ship.getCollectedBombsSize(), renderer);
}

float Game::calculateDeltaTime()
{
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime = (currentTime - lastUpdateTime) / 1000.0f;
    lastUpdateTime = currentTime;
    return deltaTime;
}

bool Game::updateGameState()
{
    if (gameState == GameState::STATE_IN_MENU)
    {
        myGameMenu.update(isRunning, gameState, MyInputHandler);
    }

    // Check if player just died and present menu with score
    if (life == 0 && gameState == GameState::STATE_IN_GAME)
    {
        gameState = GameState::STATE_IN_MENU;

        myGameMenu.setScore(score);
        int oldHighscore = myGameSave.getHighscore();
        if (score > oldHighscore)
        {
            myGameMenu.setHighscore(score);
            myGameSave.setHighscore(score);
            myGameSave.writeFile();
        }
    }

    bool pausePressed = MyInputHandler.getControlBools().pausePressed;
    if (pausePressed && gameState == GameState::STATE_IN_GAME && newPausePress)
    {
        newPausePress = false;
        gameState = GameState::STATE_PAUSE;
    }
    else if (pausePressed && gameState == GameState::STATE_PAUSE && newPausePress)
    {
        newPausePress = false;
        gameState = GameState::STATE_IN_GAME;
    }
    else if (!pausePressed)
    {
        newPausePress = true;
    }

    return gameState == GameState::STATE_IN_GAME;
}

void Game::Render()
{
    if (gameState == GameState::STATE_IN_MENU || gameState == GameState::STATE_RESET)
    {
        myGameMenu.render();
        return;
    }

    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);
    SDL_Rect playArea = {0, 0, windowWidth, windowHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &playArea);

    gameBackground.Render(renderer);

    //ship.render(renderer);
    gameObjectPtrs.push_back(&ship);

    for (Bomb &bomb : Bombs)
    {
        //bomb.render(renderer);
        gameObjectPtrs.push_back(&bomb);
    }

    for (Asteroid &asteroid : Asteroid::asteroids)
    {
        //asteroid.render(renderer);
        gameObjectPtrs.push_back(&asteroid);
    }

    for (Shot &shot : Shot::shots)
    {
        //shot.render(renderer);
        gameObjectPtrs.push_back(&shot);
    }

    for (auto objectIt = gameObjectPtrs.begin(); objectIt != gameObjectPtrs.end(); objectIt++)
    {
        (*objectIt)->render(renderer);
    }

    gameObjectPtrs.clear();


    UIScore->render(renderer);
    UILives->render(renderer);
    UIBomb->render(renderer);
    UIFPS->render(renderer);

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
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    std::cout << "Game Cleaned" << std::endl;
}

void Game::Reset()
{
    GameObject::resetId();
    Asteroid::asteroids.clear();
    Shot::shots.clear();
    Bombs.clear();

    gameObjects.clear();

    timeSinceLastAsteroidWave = 0;
    asteroidWave = 1;

    score = 0;
    life = 3;
    bombCount = 0;

    ship = Ship(windowWidth / 2, windowHeight / 2, 50, shipTex);
    gameObjects.push_back(ship);

    initAsteroids(ship, windowWidth, windowHeight);

    lastUpdateTime = SDL_GetTicks();

    gameState = GameState::STATE_IN_GAME;
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