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

        int id;
        int width, height;
        float colRadius;

        bool isVisible = true;

        SDL_FPoint velocity = {0, 0};
        SDL_FPoint midPos   = {0, 0};

        GameObject() : id(newId++) {};
        static void resetId(){newId = 1;};
};


class Ship : public GameObject
{
    private:
        // movement values
        float velocityMax       = 1000;
        float velocityDecay     = 50;
        float roatatingSpeed    = 180.0;
        float thrust            = 350.0;

        // collision values
        float sizeToCollisonRadiusRatio = 0.6;

        // shooting values
        float shotVelocity      = 1000;
        float shotCounter       = 0.0f;
        float shotCounterDecay  = 150.0;
        float maxShotCounter    = 1000.0f;
        float shipCooldown      = maxShotCounter/2;
        bool canShoot           = true;
        Uint32 timeLastShot;
    
        // animation values
        float respawnTime = 3;
        float timeNotVisible = 0;        
        int animationCounter;
        unsigned timeBetweenSprites = 300;
        Uint32 timeLastUpdated;
        
    public:

        float shipAngle = 0;

        Ship();
        Ship(float midPosX, float midPosY, int size);

        void update(ControlBools controlBools, int windowWidth, int windowHeight, float *deltaTime);
        void render(SDL_Renderer*renderer, SDL_Texture *shipTex);
        void shoot();
        void respawn(SDL_Renderer *renderer);

        float getMaxVelocity(){return velocityMax;};
        int getShotCounter(){return shotCounter;};
        int getMaxShotCounter(){return maxShotCounter;};

        bool getCanShoot(){return canShoot;};
        float getShotVelocity(){return shotVelocity;};
};


enum class AsteroidSizeType { Small, Medium };


class Asteroid : public GameObject
{
    public:
        AsteroidSizeType sizeType;
        Asteroid(float midPosX, float midPosY, SDL_FPoint velocity, AsteroidSizeType sizeType);
        void update(int windowWidth, int windowHeight, float *deltaTime);
        void render(SDL_Renderer*renderer, SDL_Texture *asteroidTexSmall, SDL_Texture *asteroidTexMedium);
        static std::list<Asteroid> asteroids;
        static float getColRadius(int size);
        static int getSize(AsteroidSizeType sizeType);
};

//void initShip(int windowWidth, int windowHeight);
bool doesCollide(GameObject firstObject, GameObject secondObject);
void asteroidsCollide(GameObject &firstObject, GameObject &secondObject);
void handleDestruction(Asteroid destoryedAsteroid);
void spawnAsteroid(int xPos, int yPos, SDL_FPoint velocity, AsteroidSizeType sizeType, std::list<GameObject> gameobjects);

class Shot : public GameObject
{
    private:
        int life;
        float vAngle = 0;
    public:
        static std::list<Shot> shots;
        Shot(float midPosX, float midPosY, SDL_FPoint velocity, float shotHeadingAngle);
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
        Bomb(int xPos, int yPos, SDL_FPoint velocity);
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


SDL_FPoint calcPosIfLeaving(SDL_FPoint midPos, float radius, int windowWidth, int windowHeight);

#endif



