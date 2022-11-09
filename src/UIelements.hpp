#ifndef UIELEMENTS_HPP
#define UIELEMENTS_HPP

#include "SDL2/SDL.h"
#include "gameobjects.hpp"

#include <iostream>


class ShotMeter
{
    private:
        SDL_Rect position;
        SDL_Rect background1;
        SDL_Rect background2;
        SDL_Rect meterBar;
        int xOffset, yOffset;
        void reconstruct(SDL_Rect position, Ship *ship);
    public:
        ShotMeter(){};
        ShotMeter(Ship *ship, int xOffset, int yOffset, int width, int height);
        void update(int shotCounter, int maxShotCounter, Ship *ship);
        void render(SDL_Renderer *renderer, bool canShoot);
};


#endif 