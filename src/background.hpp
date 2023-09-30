#pragma once

#include "SDL2/SDL.h"
#include <list>
#include <vector>

class backgroundPoint
{
public:
    backgroundPoint();
    backgroundPoint(float xPos, float yPos);

    // float xPos, yPos;
    bool onOrigin;
    SDL_FPoint currentPos;
    SDL_Point originPos;
    SDL_Point renderPos;

    void returnToOrigin(float deltaTime);
    void moveOut(class GameObject colObject);

    // render values
    float pointSizeScale = 1.5;
    void render(SDL_Renderer *renderer);

private:
    Uint8 color[4];

    // returnToOrigin function values
    float squareDistanceToOrigin, distanceToOrigin;
    float minReturnVelocity = 0.5;
    float distanceVelocityFunctionSteepness = 0.005;
    float putOnOriginDistance = 4.0;
};

class background
{
public:
    background();
    background(int windowWidth, int windowHeight);

    void update(const std::list<class GameObject> &colObjects, float deltaTime);
    void render(SDL_Renderer *renderer);

private:
    static const int divider = 100;
    int width;
    int height;

    backgroundPoint backgroundPoints[divider][divider];
    float pointAreaWidth;
    float pointAreaHeight;
};
