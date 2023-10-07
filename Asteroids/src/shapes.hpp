#pragma once

#include "SDL.h"
#include <iostream>
#include <vector>

void drawCircle(SDL_Renderer *renderer, int x0, int y0, int radius);
void drawTriangle(SDL_Renderer *renderer, float x0, float y0, float m_width, float m_height, float m_rotation, SDL_Color color);
