#ifndef MENU_HPP
#define MENU_HPP

#include <list>

#include "SDL2/SDL.h"
#include "game.hpp"
#include "handleinput.hpp"



class GameMenu 
{
    private:
        int width, height;
        TTF_Font *font;
        SDL_Rect startButtonRect;
        SDL_Rect startButtonTextRect;
        SDL_Rect scoreTextRect;
        SDL_Texture *startButtonTexture;
        SDL_Texture *scoreTextTexture;
        SDL_Renderer *renderer;
    public:
        GameMenu(){};
        GameMenu(TTF_Font *font, SDL_Renderer *renderer, int width, int height);
        void update(GameState *state, ControlBools *controlBools);
        void render();
        int score;
        int highscore;
};

#endif // MENU_HPP