#pragma once

#include <vector>
#include <list>
#include "SDL.h"
#include "vector2.hpp"

#include "gameobjects.hpp"
#include "game.hpp"

Asteroid InitSingleAsteroid(std::list<GameObject*> &gameObjects, int windowWidth, int windowHeight, Asteroid::SizeType sizeType);
void InitAsteroids(int windowWidth, int windowHeight);

Vec2 GetRandomVelocity(float minVelocity, float maxVelocity);
Vec2  GetRandomPosition(int windowWidth, int windowHeight, float m_colRadius, const std::list<GameObject*> &gameObjects);
