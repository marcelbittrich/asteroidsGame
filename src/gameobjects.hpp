#ifndef GAMEOBJECTS_HPP
#define GAMEOBJECTS_HPP

#include <iostream>
#include <math.h>
#include <vector>
#include <list>
#include <stdexcept>

#include "handleinput.hpp"
#include "shapes.hpp"


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
        static void resetId(){newId = 1;};
};


class Ship : public GameObject
{
    private:
        float vMax = 20;
        float roatatingSpeed = 3.0;
        float thrust = 0.10;
        Uint32 lastShot;
        Uint32 lastUpdated;
        int animationCounter;
        float shotCounter = 0.0f;
        float shotDecay = 150.0;
        float maxShotCounter = 1000.0f;
        float timeNotVisible = 0;
    public:
        float shipAngle = 0;
        Ship();
        Ship(float midPosX, float midPosY, int size);
        void update(ControlBools controlBools, int windowWidth, int windowHeight, float *deltaTime);
        void render(SDL_Renderer*renderer, SDL_Texture *shipTex);
        void shoot();
        void respawn(SDL_Renderer *renderer);
        float getMaxVelocity(){return vMax;};
        int getShotCounter(){return shotCounter;};
        int getMaxShotCounter(){return maxShotCounter;};
        bool canShoot = true;
};


enum class AsteroidSizeType { Small, Medium };


class Asteroid : public GameObject
{
    public:
        AsteroidSizeType sizeType;
        Asteroid(float midPosX, float midPosY, std::vector<float> velocity, AsteroidSizeType sizeType);
        void update(int windowWidth, int windowHeight, float *deltaTime);
        void render(SDL_Renderer*renderer, SDL_Texture *asteroidTexSmall, SDL_Texture *asteroidTexMedium);
        static std::list<Asteroid> asteroids;
        static float getColRadius(int size);
        static int getSize(AsteroidSizeType sizeType);
};

//void initShip(int windowWidth, int windowHeight);
bool doesCollide(GameObject firstObject, GameObject secondObject);
void asteroidsCollide(GameObject &firstObject, GameObject &secondObject);
void handleDistruction(Asteroid destoryedAsteroid);
void spawnAsteroid(int xPos, int yPos, std::vector<float> velocity, AsteroidSizeType sizeType, std::list<GameObject> gameobjects);

class Shot : public GameObject
{
    private:
        int life;
        float vAngle = 0;
    public:
        static std::list<Shot> shots;
        Shot(float midPosX, float midPosY, std::vector<float> velocity, float shotHeadingAngle);
        Uint32 creationTime;
        void update(int windowWidth, int windowHeight, float *deltaTime);
        void render(SDL_Renderer*renderer, SDL_Texture *shotTex);
};

bool shotIsToOld (Shot shot);

class Bomb : public GameObject
{
    private:
        float angle = 0.0f;
    public:
        static std::list<Bomb> bombs;
        static std::list<Bomb*> pCollectedBombs;
        Bomb(int xPos, int yPos, std::vector<float> velocity);
        Uint32 creationTime;
        Uint32 ignitionTime;
        void update(int windowWidth, int windowHeight, float *deltaTime, Ship *ship);
        void render(SDL_Renderer*renderer, SDL_Texture *bombTex);
        void collect();
        void explode();
        bool isDead = false;
        bool isExploding = false;
        bool isCollected = false;
};


std::vector<float> calcPosIfLeaving(std::vector<float> midPos, float radius, int windowWidth, int windowHeight);

#endif



