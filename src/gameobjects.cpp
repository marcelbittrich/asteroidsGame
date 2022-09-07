#include "gameobjects.hpp"
#include <math.h>

Asteroid::Asteroid(double xPos, double yPos, int width, int height)
{
    this->xPos = xPos;
    this->yPos = yPos;
    this->width = width;
    this->height = height;

    rect = getRect();
}

SDL_Rect Asteroid::getRect()
{
    SDL_Rect rect;
    rect.w = width;
    rect.h = height;
    rect.x = std::round(xPos);
    rect.y = std::round(yPos);
    return rect;
}

std::vector<Asteroid> asteroids = {};
