#include "SDL2/SDL.h"
#include <iostream>
#include <vector>

#ifndef SHAPES_HPP
#define SHAPES_HPP


void drawCircle (SDL_Renderer *renderer, int x0, int y0, int radius);
void drawTriangle (SDL_Renderer *renderer, float x0, float y0, float width, float height, float angle, SDL_Color color);

#endif // SHAPES_HPP
