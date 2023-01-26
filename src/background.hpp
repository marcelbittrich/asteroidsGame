#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "SDL2/SDL.h"
#include <vector>
#include <list>
#include "gameobjects.hpp"


class backgroundPoint
{
private:

    Uint8 color [4];

public:

    float   xPos, yPos;
    bool    onOrigin;

    SDL_Point originPos;
    SDL_Point renderPos;

    float vectorDistance [2]        = {0.0f, 0.0f};
    float normalizedDistance [2]    = {0.0f, 0.0f};
    float vectorChange [2]          = {0.0f, 0.0f};

    //returnToOrigin function values
    float squareDistanceToOrigin, distanceToOrigin;
    float minReturnVelocity                 = 0.5;
    float distanceVelocityFunctionSteepness = 0.005;

    //render values
    float pointSizeScale = 1.5;

    backgroundPoint();
    backgroundPoint(float xPos, float yPos);

    void update(GameObject colObject);
    void render(SDL_Renderer *renderer);
    void returnToOrigin(float *deltaTime);
};

class background
{
    static const int divider = 100;
    int width, height;

    backgroundPoint backgroundPoints [divider][divider];
    float pointAreaWidth;
    float pointAreaHeight;

public:
    background();
    background(int windowWidth, int windowHeight);

    void update(std::list<GameObject>colObjects, float *deltaTime);
    void render(SDL_Renderer *renderer);
};


#endif //BACKGROUND_HPP
