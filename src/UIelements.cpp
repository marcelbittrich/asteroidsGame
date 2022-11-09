#include "UIelements.hpp"

ShotMeter::ShotMeter (Ship *ship, int xOffset, int yOffset, int width, int height)
{
    this->xOffset = xOffset;
    this->yOffset = yOffset;
    position.w = width;
    position.h = height;

    reconstruct(position, ship);
}

void ShotMeter::reconstruct(SDL_Rect position, Ship *ship)
{   
    position.x = ship->midPos[0] + xOffset - position.w / 2;
    position.y = ship->midPos[1] + yOffset + position.h / 2;
    int borderOffset = 1;
    background1 = position;
    background2 = {background1.x + borderOffset,
                background1.y + borderOffset,
                background1.w - 2 * borderOffset,
                background1.h - 2 * borderOffset};
    meterBar = background2;
}

void ShotMeter::update(int shotCounter, int maxShotCounter, Ship *ship)
{
    reconstruct(position, ship);
    float shotMeterPercent = (std::min((((float)shotCounter / (float)maxShotCounter) * (float)maxShotCounter), (float)maxShotCounter)) / (float)maxShotCounter;
    meterBar.w = (int)(shotMeterPercent * background2.w);
}

void ShotMeter::render(SDL_Renderer *renderer, bool canShoot)
{
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderFillRect(renderer, &background1);
    SDL_SetRenderDrawColor(renderer,0,0,0,100);
    SDL_RenderFillRect(renderer, &background2);
    canShoot ? SDL_SetRenderDrawColor(renderer,0,200,0,255) : SDL_SetRenderDrawColor(renderer,200,0,0,255);
    SDL_RenderFillRect(renderer, &meterBar);
    SDL_RenderPresent(renderer);
}