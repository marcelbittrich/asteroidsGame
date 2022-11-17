#include <iostream>
#include <math.h>
#include <vector>
#include <stdexcept>
#include <list>
#include <numeric>
//#include <algorithm>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_gamecontroller.h"
#include "SDL2/SDL_ttf.h"

#include "handleinput.hpp"
#include "gameobjects.hpp"
#include "shapes.hpp"
#include "background.hpp"
#include "initialization.hpp"
#include "UIelements.hpp"
#include "menu.hpp"
#include "gamesave.hpp"

#ifndef GAME_HPP
#define GAME_HPP

#define PI 3.14159265359

enum GameState {
    STATE_IN_MENU,
    STATE_IN_GAME,
    STATE_RESET,
    STATE_PAUSE
};

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

    void handleEvents();
    void update();
    void render();
    void clean();
    void reset();

    bool isRunning;
    SDL_Renderer *renderer;

    bool running() {return isRunning;}
    Uint32 frameStart = 0; 
    Uint32 frameTime = 0;
    GameState state;
private:
    SDL_Window *window;
};

#endif /* game_hpp */
