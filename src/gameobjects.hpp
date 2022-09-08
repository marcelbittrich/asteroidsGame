#ifndef GAMEOBJECTS_HPP
#define GAMEOBJECTS_HPP

#include "game.hpp"

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

void initAsteroids(SDL_Rect shipRect, int windowWidth, int windowHeight);

extern std::vector<Asteroid> asteroids;

#endif