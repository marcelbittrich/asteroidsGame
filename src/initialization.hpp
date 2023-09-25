#pragma once

#include <vector>
#include <list>
#include "SDL2/SDL.h"

#include "gameobjects.hpp"
#include "game.hpp"

void initSingleAsteroid(std::list<GameObject> &gameObjects, int windowWidth, int windowHeight, AsteroidSizeType sizeType);
void initAsteroids(GameObject ship, int windowWidth, int windowHeight);

SDL_FPoint getRandomVelocity(float minVelocity, float maxVelocity);
SDL_Point getRandomPosition(int windowWidth, int windowHeight, float colRadius, std::list<GameObject> gameObjects);
