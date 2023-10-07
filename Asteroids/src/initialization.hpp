#pragma once

#include <vector>
#include <list>
#include "SDL.h"
#include "vector2.hpp"

#include "gameobjects.hpp"
#include "game.hpp"

void initSingleAsteroid(std::list<GameObject> &gameObjects, int windowWidth, int windowHeight, AsteroidSizeType sizeType);
void initAsteroids(GameObject ship, int windowWidth, int windowHeight);

Vec2 getRandomVelocity(float minVelocity, float maxVelocity);
Vec2  getRandomPosition(int windowWidth, int windowHeight, float m_colRadius, const std::list<GameObject> &gameObjects);
