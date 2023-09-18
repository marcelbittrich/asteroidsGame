#include <vector>
#include <list>

#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_stdinc.h"

#include "gameobjects.hpp"

#ifndef GAME_HPP
#define GAME_HPP

#define PI 3.14159265359

enum GameState
{
    STATE_IN_MENU,
    STATE_IN_GAME,
    STATE_RESET,
    STATE_PAUSE
};

class Game
{
public:
    Game();
    ~Game();
    void init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen);
    void handleEvents();
    void update();
    void render();
    void clean();
    void reset();

    GameState gameState = GameState::STATE_IN_MENU;

    void printPerformanceInfo(Uint32 updateTime, Uint32 renderTime, Uint32 loopTime, Uint32 frameTime);

private:
    bool isRunning;
    // Game window values
    int windowWidth;
    int windowHeight;
    class SDL_Window *window;
    class SDL_Renderer *renderer;
    void initWindow(const char *title, int xpos, int ypos, int width, int height, bool fullscreen);

    // Control values
    void initInputDevices();
    class InputHandler *MyInputHandler;

    // Texture values
    class SDL_Texture *shipTex;
    class SDL_Texture *asteroidTexSmall;
    class SDL_Texture *asteroidTexMedium;
    class SDL_Texture *shotTex;
    class SDL_Texture *bombTex;
    TTF_Font *font;
    TTF_Font *fontHuge;

    void initTextures();

    // Main menu values
    class GameSave *myGameSave;
    class GameMenu *myGameMenu;

    void initMenu();

    // Gameplay values
    class Ship *ship;
    class background *gameBackground;
    std::list<GameObject> colObjects;

    void initGameplay();

    int score;
    int life;
    int bombCount;
    int FPS;

    /// Values for wave spwan system
    float timeSinceLastAsteroidWave = 0;
    int asteroidWave = 1;

    /// Interaction values
    bool newClick = true;
    bool newPausePress = true;
    bool gameIsPaused = false;
    bool newBombIgnition = true;

    // UI values
    class UICounter *UIScore;
    class UICounter *UILives;
    class UICounter *UIBomb;
    class UICounter *UIFPS;
    class ShotMeter *shotMeter; // Alternative shot meter rendered below ship

    void initUI();

    // deltaTime calculation
    Uint32 lastUpdateTime;

    // Performance values
    bool showUpdateTime = false;
    bool showRenderTime = false;
    bool showLoopTime = false;
    bool showFrameTime = false;
    bool showFPS = false;

    std::vector<int> FPSVector;

    //
    // Update
    //

    float calculateDeltaTime();
    bool updateGameState();

public:
    bool getIsRunning() { return isRunning; }
};

#endif /* game_hpp */
