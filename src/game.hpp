#include <iostream>
#include <math.h>
#include <vector>
#include <stdexcept>
#include <list>
#include <numeric>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_gamecontroller.h"
#include "SDL2/SDL_ttf.h"

#include "gameobjects.hpp"
#include "shapes.hpp"
#include "background.hpp"
#include "initialization.hpp"
#include "UIelements.hpp"
#include "menu.hpp"
#include "gamesave.hpp"
#include "inputhandler.hpp"

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

    // Texture values
    class SDL_Texture *shipTex;
    class SDL_Texture *asteroidTexSmall;
    class SDL_Texture *asteroidTexMedium;
    class SDL_Texture *shotTex;
    class SDL_Texture *bombTex;
    TTF_Font *font;
    TTF_Font *fontHuge;

    // Game window values
    int windowWidth;
    int windowHeight;

    void init(const char *title, int xpos, int ypos, int width, int height, bool fullscreen);

    // Control values
    InputHandler *MyInputHandler;
    // ControlBools controlBools;

    void handleEvents();
    void update();
    void render();
    void clean();
    void reset();

    bool isRunning;
    SDL_Renderer *renderer;

    Uint32 frameStart = 0;
    Uint32 frameTime = 0;
    GameState gameState = GameState::STATE_IN_MENU;

    // Performance assessment
    bool showUpdateTime = false;
    bool showRenderTime = false;
    bool showFrameTime = false;
    bool showDelayedFrameTime = false;
    bool showFPS = false;

    void printPerformanceInfo(Uint32 updateTime, Uint32 renderTime, Uint32 loopTime);

private:
    SDL_Window *window;

public:
    bool getIsRunning() { return isRunning; }
};

#endif /* game_hpp */
