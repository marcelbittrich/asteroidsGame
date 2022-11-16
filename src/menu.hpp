#ifndef MENU_HPP
#define MENU_HPP

#include <list>

#include "SDL2/SDL.h"
#include "game.hpp"
#include "handleinput.hpp"



class GameMenu 
{
    private:
        std::list<SDL_Rect> menuRects;
        int width, height;
    public:
        GameMenu(){};
        GameMenu(int width, int height);
        void update(GameState *state, ControlBools *controlBools);
        void render(SDL_Renderer *renderer);
        SDL_Rect menuRect;
};

#endif // MENU_HPP