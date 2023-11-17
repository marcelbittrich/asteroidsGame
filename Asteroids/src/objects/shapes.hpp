#pragma once

#include <iostream>
#include <vector>

#include "SDL.h"

void DrawCircle(SDL_Renderer *renderer, int x0, int y0, int radius);
void DrawTriangle(SDL_Renderer *renderer, float x0, float y0, float width, float height, float rotation, SDL_Color color);
