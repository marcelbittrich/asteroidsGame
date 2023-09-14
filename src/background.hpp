#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "SDL2/SDL.h"
#include <list>

class backgroundPoint
{
private:
    Uint8 color[4];

public:
    // float xPos, yPos;
    bool onOrigin;
    SDL_FPoint currentPos;
    SDL_Point originPos;
    SDL_Point renderPos;

    float vectorChange[2] = {0.0f, 0.0f};

    // returnToOrigin function values
    float squareDistanceToOrigin, distanceToOrigin;
    float minReturnVelocity = 0.5;
    float distanceVelocityFunctionSteepness = 0.005;
    float putOnOriginDistance = 4.0;

    // render values
    float pointSizeScale = 1.5;

    backgroundPoint();
    backgroundPoint(float xPos, float yPos);

    void update(class GameObject colObject);
    void render(SDL_Renderer *renderer);
    void returnToOrigin(float *deltaTime);
    float squareDistance(SDL_FPoint PositionA, SDL_FPoint PositionB);
};

class background
{
    static const int divider = 100;
    int width, height;

    backgroundPoint backgroundPoints[divider][divider];
    float pointAreaWidth;
    float pointAreaHeight;

public:
    background();
    background(int windowWidth, int windowHeight);

    void update(std::list<class GameObject> colObjects, float *deltaTime);
    void render(SDL_Renderer *renderer);
};

#endif // BACKGROUND_HPP
