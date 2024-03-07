#pragma once

#include <iostream>
#include <vector>

#include "SDL.h"
#include "../vector2.hpp"

void DrawCircle(SDL_Renderer *renderer,const Vec2& position, const SDL_Color& color, int radius);
void DrawTriangle(SDL_Renderer *renderer, const Vec2& position, float width, float height, float rotation, SDL_Color color);
