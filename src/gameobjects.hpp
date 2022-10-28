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
        float colRadius;
        std::vector<float> velocity {0, 0};
        std::vector<float> midPos {0, 0};
        bool isVisible = true;
};


class Ship : public Gameobject
{
    private:
        float vMax = 20;
        float roatatingSpeed = 2.0;
        float thrust = 0.05;
        Uint32 lastUpdated;
        int animationCounter;
    public:
        float shipAngle = 0;
        Ship();
        Ship(float midPosX, float midPosY, int size);
        void update(ControlBools controlBools, int windowWidth, int windowHeight, float *deltaTime);
        void render(SDL_Renderer*renderer, SDL_Texture *shipTex);
        float getMaxVelocity(){return vMax;};
};


enum class AsteroidSizeType { Small, Medium };


class Asteroid : public Gameobject
{
    public:
        AsteroidSizeType sizeType;
        Asteroid(AsteroidSizeType sizeType);
        void update(int windowWidth, int windowHeight, float *deltaTime);
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
        float vAngle = 0;
    public:
        Shot(float midPosX, float midPosY, std::vector<float> velocity, float shotHeadingAngle);
        Uint32 creationTime;
        void update(int windowWidth, int windowHeight, float *deltaTime);
        void render(SDL_Renderer*renderer, SDL_Texture *shotTex);
};

void shoot(Ship ship);
bool shotIsToOld (Shot shot);
std::vector<float> calcPosIfLeaving(std::vector<float> midPos, float radius, int windowWidth, int windowHeight);


//extern Ship ship;
extern std::vector<Shot> shots;
extern std::vector<Gameobject> colObjects;
extern std::vector<Asteroid> asteroids;

#endif



