#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "SDL2/SDL.h"
#include <vector>
#include "gameobjects.hpp"


class bgPoint
{
public:
    double xPos, yPos;

    SDL_Point originPos;
    SDL_Point renderPos;

    bgPoint(int xPos, int yPos);
    
    void update(Gameobject colObject);
    void render(SDL_Renderer *renderer);
    void returnToOrigin();
    bool onOrigin;

    std::vector<int> color = {255,0,0,255};
};

class background
{
private:
    int width, height, divider;
    std::vector<bgPoint>pointCloud;
public:
    background();
    background(int windowWidth, int windowHeight, int divider);
    void update(std::vector<Gameobject>colObjects);
    void render(SDL_Renderer *renderer);

    //debug
    void getPointPosition(int vectorPos);
};


#endif //BACKGROUND_HPP
