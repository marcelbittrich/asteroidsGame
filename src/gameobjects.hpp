#ifndef GAMEOBJECTS_HPP
#define GAMEOBJECTS_HPP

#include <iostream>
#include <math.h>
#include <vector>
#include <stdexcept>

#include "handleinput.hpp"


class Gameobject
{
    private:
        static int newId;
    public:
        int id;
        Gameobject() : id(newId++) {};
        double xPos, yPos;
        double colRadius;
        std::vector<double> velocity {0, 0};
        std::vector<double> midPos {0, 0};
        SDL_Rect rect;
        bool isVisible = true;
        //virtual void draw(); TODO 
        //virtual void update(); TODO
};


class Ship : public Gameobject
{
    private:
        double vMax = 20;
        double roatatingSpeed = 2.0;
        double thrust = 0.05;
        int width, height;
        SDL_Rect getRect();
    public:
        double shipAngle;
        Ship();
        Ship(double xPos, double yPos, int width, int height);
        void update(ControlBools controlBools, int windowWidth, int windowHeight);
        double getMaxVelocity(){return vMax;};
};

class Asteroid : public Gameobject
{
    private:
        SDL_Rect getRect();
    public:
        int size;
        //SDL_Rect rect;
        Asteroid(double xPos, double yPos, int size);
        void update(int windowWidth, int windowHeight);
};

void initAsteroids(SDL_Rect shipRect, int windowWidth, int windowHeight);
//void initShip(int windowWidth, int windowHeight);
bool doesCollide(Gameobject firstObject, Gameobject secondObject);
void asteroidsCollide(Gameobject &firstObject, Gameobject &secondObject);

class Shot : public Gameobject
{
    private:
        SDL_Rect getRect();
        int size;
        int life;
        double vAngle;
    public:
        Shot(std::vector<double> midPos, std::vector<double> velocity, double shotHeadingAngle);
        Uint32 creationTime;
        void update(int windowWidth, int windowHeight);
        void render(SDL_Renderer*renderer, SDL_Texture *shotTex);
};

void shoot(Ship ship);
bool shotIsToOld (Shot shot);
std::vector<double> calcPosIfLeaving(std::vector<double> midPos, double radius, int windowWidth, int windowHeight);


//extern Ship ship;
extern std::vector<Shot> shots;
extern std::vector<Asteroid> asteroids;
extern std::vector<Gameobject> colObjects;

#endif



