#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "SDL2/SDL.h"
#include <vector>
#include <list>
#include "gameobjects.hpp"


class bgPoint
{
private:
    Uint8 color [4];
public:
    float xPos, yPos;
    bool onOrigin;
    SDL_Point originPos;
    SDL_Point renderPos;
    bgPoint();
    bgPoint(int xPos, int yPos);
    
    void update(GameObject colObject);
    void render(SDL_Renderer *renderer);
    void returnToOrigin(float *deltaTime);
};

class background
{
private:
    int width, height, divider;
    std::vector<std::vector<bgPoint>>pointCloud;
public:
    background();
    background(int windowWidth, int windowHeight, int divider);
    void update(std::list<GameObject>colObjects, float *deltaTime);
    void render(SDL_Renderer *renderer);

    //debug
    void getPointPosition(int vectorPos);
};


#endif //BACKGROUND_HPP
