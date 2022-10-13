#include "initialization.hpp"

Ship initShip(int windowWidth, int windowHeight){
    int size = 50;
    double shipPosX = windowWidth/2-size/2;
    double shipPosY = windowHeight/2-size/2;
    return Ship(shipPosX, shipPosY, size);
}

SDL_Point getRandomPosition(
    int windowWidth,
    int windowHeight,
    int objectWidth,
    int objectHeight,
    std::vector<SDL_Rect> gameObjects
) {
    int maxTries = 10000;
    double minDistance = 50;
    for (int i = 0; i < maxTries; i++) {
        int x = rand() % windowWidth;
        int y = rand() % windowHeight;
        bool success = true;
        for (SDL_Rect gameObject : gameObjects)
        {
            int gameObjectCenterX = gameObject.x + gameObject.w / 2;
            int gameObjectCenterY = gameObject.y + gameObject.h / 2;
            double distance = sqrt(std::pow(gameObjectCenterX - x, 2) + std::pow(gameObjectCenterY - y, 2));
            if (distance < minDistance) {
                std::cout << "RandomPosTry: " << i+1 << std::endl;
                success = false;
                break;
            }
        }
        if (success) {
            int new_x = std::round(x - objectWidth / 2);
            int new_y = std::round(y - objectHeight / 2);
            SDL_Point point = {new_x, new_y};
            return point;
        }
    }
    throw std::runtime_error("Max tries for getRandomPosition exceeded!");
}

double randomSign(){
    if(rand()% 100 <= 49) return -1.0f;
    return 1.0f; 
}


void initSingleAsteroid(std::vector<SDL_Rect> &gameObjects, int windowWidth, int windowHeight, int asteroidSize)
{
    int asteroidMinVel = 0;
    int asteroidMaxVel = 100;
    double asteroidVelMulti = 0.1;
    SDL_Point randomPosition = getRandomPosition(
        windowWidth, windowHeight, asteroidSize, asteroidSize, gameObjects
    );
    Asteroid asteroid = Asteroid(randomPosition.x, randomPosition.y, asteroidSize);  
    asteroid.velocity = {randomSign()*asteroidVelMulti*((double)(rand() % (asteroidMaxVel-asteroidMinVel) + asteroidMinVel))/10,randomSign()*asteroidVelMulti*((double)(rand() % (asteroidMaxVel-asteroidMinVel) + asteroidMinVel))/10};
    std::cout << "Asteroidgeschwidigkeit: " << asteroid.velocity[0] << ", " << asteroid.velocity[1] <<std::endl; 
    asteroids.push_back(asteroid);
    gameObjects.push_back(asteroid.rect);
    colObjects.push_back(asteroid);
}

void initAsteroids(SDL_Rect shipRect, int windowWidth, int windowHeight)
{
    int asteroidAmountSmall = 10;
    int asteroidAmountMedium = 10;
    std::vector<SDL_Rect> gameObjects = {shipRect};
    for (int i=0; i < asteroidAmountSmall; i++)
    {
        initSingleAsteroid(gameObjects, windowWidth, windowHeight, 50);
    }
    for (int i=0; i < asteroidAmountMedium; i++)
    {
        initSingleAsteroid(gameObjects, windowWidth, windowHeight, 100);
    }
}
