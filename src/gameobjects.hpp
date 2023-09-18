#ifndef GAMEOBJECTS_HPP
#define GAMEOBJECTS_HPP

#include <iostream>
#include <math.h>
#include <vector>
#include <list>
#include <stdexcept>

#include "inputhandler.hpp"
#include "shapes.hpp"

class GameObject
{
private:
    static int newId;

protected:
    SDL_Rect getRenderRect();

public:
    int id;
    int width;
    int height;
    float colRadius;

    bool isVisible = true; // indicator, false during respawn

    SDL_FPoint velocity = {0, 0};
    SDL_FPoint midPos = {0, 0};

    GameObject() : id(newId++){};
    static void resetId() { newId = 1; };
};

class Ship : public GameObject
{
public:
    Ship();
    Ship(int midPosX, int midPosY, int size);

    void update(InputHandler *MyInputHandler, int windowWidth, int windowHeight, float deltaTime);
    void render(SDL_Renderer *renderer, SDL_Texture *shipTex);
    void reset(SDL_Renderer *renderer);
    void respawn(SDL_Renderer *renderer);

private:
    // Update shooting capability and ship visibility.
    // Ship is not visible during respawn.
    float respawnTime = 3;
    float timeNotVisible = 0;
    void updateVisibility(float deltaTime);

    // movement values
    float velocityMax = 1000;
    float velocityDecay = 50;
    float rotation = 0; // Current rotation in degree
    float roatatingSpeed = 180.0;
    float thrust = 350.0;
    void updateTransform(InputHandler *MyInputHandler, int windowWidth, int windowHeight, float deltaTime);
    void updateAnimation(InputHandler *MyInputHandler, float deltaTime);

    // shooting values
    float shotVelocity = 1000;
    float shotCounter = 0.0f;
    float shotCounterDecay = 150.0;
    float maxShotCounter = 1000.0f;
    float shipCooldownThreshold = maxShotCounter / 2;

    bool canShoot = true; // will set to false during respawn
    Uint32 timeLastShot;
    int timeBetweenShots = 250;
    void shoot();
    void createShot();

    // Animation values
    int spriteWidth = 300;
    int spriteHeight = 300;
    int spriteCount = 3;  // Number of different sprites in texture
    int animationCounter; // Used to select sprite
    unsigned timeBetweenSprites = 300;
    Uint32 timeLastUpdated;
    void renderShotMeter(SDL_Renderer *renderer);
    void renderShip(SDL_Renderer *renderer, SDL_Texture *shipTex);

    // collision values
    float sizeToCollisonRadiusRatio = 0.6;

public:
    float getMaxVelocity() { return velocityMax; };
    int getShotCounter() { return shotCounter; };
    int getMaxShotCounter() { return maxShotCounter; };
    bool getCanShoot() { return canShoot; };
    float getShotVelocity() { return shotVelocity; };
};

enum class AsteroidSizeType
{
    Small,
    Medium
};

class Asteroid : public GameObject
{
public:
    AsteroidSizeType sizeType;
    Asteroid(float midPosX, float midPosY, SDL_FPoint velocity, AsteroidSizeType sizeType);
    void update(int windowWidth, int windowHeight, float deltaTime);
    void render(SDL_Renderer *renderer, SDL_Texture *asteroidTexSmall, SDL_Texture *asteroidTexMedium);
    static std::list<Asteroid> asteroids;
    static float getColRadius(int size);
    static int getSize(AsteroidSizeType sizeType);
};

// void initShip(int windowWidth, int windowHeight);
bool doesCollide(GameObject firstObject, GameObject secondObject);
void asteroidsCollide(GameObject &firstObject, GameObject &secondObject);
void handleDestruction(Asteroid destoryedAsteroid);
void spawnAsteroid(int xPos, int yPos, SDL_FPoint velocity, AsteroidSizeType sizeType, std::list<GameObject> gameobjects);

class Shot : public GameObject
{
public:
    Shot(float midPosX, float midPosY, SDL_FPoint velocity, float shotHeadingAngle);
    void update(int windowWidth, int windowHeight, float deltaTime);
    void render(SDL_Renderer *renderer, SDL_Texture *shotTex);

private:
    int life;
    float vAngle = 0;

public:
    static std::list<Shot> shots;

    Uint32 creationTime;
};

bool shotIsToOld(Shot shot);

class Bomb : public GameObject
{
private:
    float angle = 0.0f;

public:
    static std::list<Bomb> bombs;
    static std::list<Bomb *> pCollectedBombs;
    Bomb(int xPos, int yPos, SDL_FPoint velocity);
    Uint32 creationTime;
    Uint32 ignitionTime;
    void update(int windowWidth, int windowHeight, float deltaTime, Ship *ship);
    void render(SDL_Renderer *renderer, SDL_Texture *bombTex);
    void collect();
    void explode();
    bool isDead = false;
    bool isExploding = false;
    bool isCollected = false;
};

SDL_FPoint calcPosIfLeaving(SDL_FPoint midPos, float radius, int windowWidth, int windowHeight);

#endif
