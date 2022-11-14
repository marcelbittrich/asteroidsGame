#ifndef INITIALIZATION_HPP
#define INITIALIZATION_HPP
 
#include <vector>
#include <list>
#include "SDL2/SDL.h"

#include "gameobjects.hpp"
#include "game.hpp"

Ship initShip(int windowWidth, int windowHeight);
void initSingleAsteroid(std::list<GameObject> &gameObjects, int windowWidth, int windowHeight, AsteroidSizeType sizeType);
void initAsteroids(GameObject ship, int windowWidth, int windowHeight);

std::vector<float> getRandomVelocity (float minVelocity, float maxVelocity);

# endif 


