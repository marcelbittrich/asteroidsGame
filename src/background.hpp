#pragma once

#include "SDL.h"
#include <list>
#include <vector>

class BackgroundPoint
{
public:
    BackgroundPoint();
    BackgroundPoint(float xPos, float yPos);
    SDL_FPoint currentPos;
    SDL_Point originPos;
    SDL_Point renderPos;
    Uint8 color[4];
    bool onOrigin;
};

class Background
{
public:
    Background();
    Background(int windowWidth, int windowHeight);
    void update(const std::list<class GameObject> &colObjects, float deltaTime);
    void render(SDL_Renderer *renderer);

private:
    static const int divider = 100;
    float pointSizeScale = 2.f;
    int width = 0;
    int height = 0;

    BackgroundPoint backgroundPoints[divider][divider];
    float pointAreaWidth = 0;
    float pointAreaHeight = 0;

    std::vector<uint32_t> horizontalIter, verticalIter;

    void returnPointToOrigin(BackgroundPoint &point, float deltaTime);
    void movePointOut(BackgroundPoint &point, class GameObject colObject);

    float minReturnVelocity = 0.5;
    float distanceVelocityFunctionSteepness = 0.005;
    float putOnOriginDistance = 4.0;

    void setPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
};
