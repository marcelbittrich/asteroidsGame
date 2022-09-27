#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "SDL2/SDL.h"
#include <vector>
#include "gameobjects.hpp"


class bgPoint
{
private:
    SDL_Point originPos;
    SDL_Point renderPos;
    double xPos, yPos;
    std::vector<int> color = {255,0,0,255};
public:
    bgPoint(int xPos, int yPos);
    void update(std::vector<Gameobject>colObjects);
    void render(SDL_Renderer *renderer);
    
};

class background
{
private:
    int width, height;
    std::vector<bgPoint>pointCloud;
public:
    background();
    background(int windowWidth, int windowHeight, int divider);
    void update(std::vector<Gameobject>colObjects);
    void render(SDL_Renderer *renderer);
};




#endif //BACKGROUND_HPP
