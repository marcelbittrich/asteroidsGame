#include <stdexcept>
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

SDL_Point getRandomPosition(
    int windowWidth,
    int windowHeight,
    int objectWidth,
    int objectHeight,
    std::vector<SDL_Rect> gameObjects
) {
    int maxTries = 10000;
    double minDistance = 50;
    for (int i = 0; i < maxTries; i++) {
        int x = rand() % windowWidth;
        int y = rand() % windowHeight;
        bool success = true;
        for (SDL_Rect gameObject : gameObjects)
        {
            int gameObjectCenterX = gameObject.x + gameObject.w / 2;
            int gameObjectCenterY = gameObject.y + gameObject.h / 2;
            double distance = sqrt(std::pow(gameObjectCenterX - x, 2) + std::pow(gameObjectCenterY - y, 2));
            if (distance < minDistance) {
                success = false;
                break;
            }
        }
        if (success) {
            int new_x = std::round(x - objectWidth / 2);
            int new_y = std::round(y - objectHeight / 2);
            SDL_Point point = {new_x, new_y};
            return point;
        }
    }
    throw std::runtime_error("Max tries for getRandomPosition exceeded!");
}

void initAsteroids(SDL_Rect shipRect, int windowWidth, int windowHeight)
{
    int asteroidAmount = 100;
    int asteroidSize = 50;
    std::vector<SDL_Rect> gameObjects = {shipRect};
    for (int i=0; i < asteroidAmount; i++)
    {
        SDL_Point randomPosition = getRandomPosition(
            windowWidth, windowHeight, asteroidSize, asteroidSize, gameObjects
        );
        Asteroid asteroid = Asteroid(randomPosition.x, randomPosition.y, asteroidSize, asteroidSize);
        asteroids.push_back(asteroid);
        gameObjects.push_back(asteroid.rect);
    }
}
