#ifndef GAMEOBJECTS_HPP
#define GAMEOBJECTS_HPP

#include <iostream>
#include <math.h>
#include <vector>
#include <stdexcept>

#include "handleinput.hpp"


class Gameobject
{
    public:  
        double xPos, yPos;
        double col_radius;
        std::vector<double> velocity {0, 0}; 
};

class Ship : public Gameobject
{
    private:
        double v_max = 20;
        double roatatingSpeed = 2.0;
        double thrust = 0.05;
        int width, height;
        SDL_Rect getRect();
    public:
        double shipAngle;
        SDL_Rect rect;
        Ship();
        ~Ship();
        Ship(double xPos, double yPos, int width, int height);
        void update(ControlBools controlBools, int windowWidth, int windowHeight);
};

class Asteroid : public Gameobject
{
    private:
        int width, height;
        SDL_Rect getRect();
    public:
        SDL_Rect rect;
        Asteroid(double xPos, double yPos, int width, int height);
};

void initAsteroids(SDL_Rect shipRect, int windowWidth, int windowHeight);
//void initShip(int windowWidth, int windowHeight);
bool doesCollide(Gameobject firstObject, Gameobject secondObject);

//extern Ship ship;
extern std::vector<Asteroid> asteroids;

#endif