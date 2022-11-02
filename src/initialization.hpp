#ifndef INITIALIZATION_HPP
#define INITIALIZATION_HPP
 
#include <vector>
#include "SDL2/SDL.h"

#include "gameobjects.hpp"
#include "game.hpp"

Ship initShip(int windowWidth, int windowHeight);
void initAsteroids(GameObject ship, int windowWidth, int windowHeight);

# endif 


