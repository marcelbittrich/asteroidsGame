#pragma once

#include <vector>

#include "../vector2.hpp"

Vec2 GetFreeRandomPosition(Vec2 windowDimensions, float m_colRadius, const std::vector<class GameObject*> &gameObjects);
Vec2 GetRandomVelocity(float minVelocity, float maxVelocity);

float randomSign();
float getRandomValue(float min, float max);

