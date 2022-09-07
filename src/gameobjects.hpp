#ifndef GAMEOBJECTS_HPP
#define GAMEOBJECTS_HPP

#include "SDL2/SDL.h"
#include <SDL2/SDL_image.h>
#include <vector>

class Asteroid
{
    private:
        double xPos, yPos;
        int width, height;
        SDL_Rect getRect();
    public:
        SDL_Rect rect;
        Asteroid(double xPos, double yPos, int width, int height);
};


extern std::vector<Asteroid> asteroids;

#endif