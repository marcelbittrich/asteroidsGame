#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>
#include <math.h>
#include <vector>
#include <stdexcept>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_gamecontroller.h"

#include "handleinput.hpp"
#include "gameobjects.hpp"
#include "shapes.hpp"
#include "background.hpp"

#define PI 3.14159265359

class Game {
public:
    Game();
    ~Game();

    void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

    void handleEvents();
    void update();
    void render();
    void clean();

    bool running() {return isRunning;}
    Uint32 frameStart = 0; 
    Uint32 frameTime = 0;
private:
    bool isRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;
};



#endif /* game_hpp */
