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
    protected:
        SDL_Rect getRect();
    public:
        int width;
        int height;
        int id;
        Gameobject() : id(newId++) {};
        // double xPos, yPos;
        double colRadius;
        std::vector<double> velocity {0, 0};
        std::vector<double> midPos {0, 0};
        bool isVisible = true;
};


class Ship : public Gameobject
{
    private:
        double vMax = 20;
        double roatatingSpeed = 2.0;
        double thrust = 0.05;
        Uint32 lastUpdated;
        int animationCounter;
    public:
        double shipAngle;
        Ship();
        Ship(double midPosX, double midPosY, int size);
        void update(ControlBools controlBools, int windowWidth, int windowHeight);
        void render(SDL_Renderer*renderer, SDL_Texture *shipTex);
        double getMaxVelocity(){return vMax;};
};


enum class AsteroidSizeType { Small, Medium };


class Asteroid : public Gameobject
{
    public:
        AsteroidSizeType sizeType;
        Asteroid(AsteroidSizeType sizeType);
        void update(int windowWidth, int windowHeight);
        void render(SDL_Renderer*renderer, SDL_Texture *asteroidTexSmall, SDL_Texture *asteroidTexMedium);
};

//void initShip(int windowWidth, int windowHeight);
bool doesCollide(Gameobject firstObject, Gameobject secondObject);
void asteroidsCollide(Gameobject &firstObject, Gameobject &secondObject);
void handleDistruction(Asteroid destoryedAsteroid);

class Shot : public Gameobject
{
    private:
        int life;
        double vAngle;
    public:
        Shot(double midPosX, double midPosY, std::vector<double> velocity, double shotHeadingAngle);
        Uint32 creationTime;
        void update(int windowWidth, int windowHeight);
        void render(SDL_Renderer*renderer, SDL_Texture *shotTex);
};

void shoot(Ship ship);
bool shotIsToOld (Shot shot);
std::vector<double> calcPosIfLeaving(std::vector<double> midPos, double radius, int windowWidth, int windowHeight);


//extern Ship ship;
extern std::vector<Shot> shots;
extern std::vector<Gameobject> colObjects;
extern std::vector<Asteroid> asteroids;

#endif



