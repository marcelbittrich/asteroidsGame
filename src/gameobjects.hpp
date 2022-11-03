#ifndef GAMEOBJECTS_HPP
#define GAMEOBJECTS_HPP

#include <iostream>
#include <math.h>
#include <vector>
#include <stdexcept>

#include "handleinput.hpp"


class GameObject
{
    private:
        static int newId;
    protected:
        SDL_Rect getRect();
    public:
        int width;
        int height;
        int id;
        GameObject() : id(newId++) {};
        float colRadius;
        std::vector<float> velocity {0, 0};
        std::vector<float> midPos {0, 0};
        bool isVisible = true;
};


class Ship : public GameObject
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


class Asteroid : public GameObject
{
    public:
        AsteroidSizeType sizeType;
        Asteroid(AsteroidSizeType sizeType);
        void update(int windowWidth, int windowHeight, float *deltaTime);
        void render(SDL_Renderer*renderer, SDL_Texture *asteroidTexSmall, SDL_Texture *asteroidTexMedium);
        static std::vector<Asteroid> asteroids;
};

//void initShip(int windowWidth, int windowHeight);
bool doesCollide(GameObject firstObject, GameObject secondObject);
void asteroidsCollide(GameObject &firstObject, GameObject &secondObject);
void handleDistruction(Asteroid destoryedAsteroid);

class Shot : public GameObject
{
    private:
        int life;
        float vAngle = 0;
    public:
        static std::vector<Shot*> shots;
        Shot(float midPosX, float midPosY, std::vector<float> velocity, float shotHeadingAngle);
        Uint32 creationTime;
        void update(int windowWidth, int windowHeight, float *deltaTime);
        void render(SDL_Renderer*renderer, SDL_Texture *shotTex);
};

void shoot(Ship ship);
bool shotIsToOld (Shot shot);
std::vector<float> calcPosIfLeaving(std::vector<float> midPos, float radius, int windowWidth, int windowHeight);


#endif



