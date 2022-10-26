#include "initialization.hpp"

Ship initShip(int windowWidth, int windowHeight){
    int size = 50;
    float midPosX = windowWidth/2;
    float midPosY = windowHeight/2;
    return Ship(midPosX, midPosY, size);
}

SDL_Point getRandomPosition(
    int windowWidth,
    int windowHeight,
    Gameobject newGameobject,
    std::vector<Gameobject> gameobjects
) {
    int maxTries = 10000;
    for (int i = 0; i < maxTries; i++) {
        int x = rand() % windowWidth;
        int y = rand() % windowHeight;
        bool success = true;
        for (const Gameobject &gameobject : gameobjects)
        {
            float distance = sqrt(std::pow(gameobject.midPos[0] - x, 2) + std::pow(gameobject.midPos[1] - y, 2));
            if (distance < gameobject.colRadius + newGameobject.colRadius) {
                std::cout << "RandomPosTry: " << i+1 << std::endl;
                success = false;
                break;
            }
        }
        if (success) {
            SDL_Point point = {x, y};
            return point;
        }
    }
    throw std::runtime_error("Max tries for getRandomPosition exceeded!");
}

float randomSign(){
    if(rand()% 100 <= 49) return -1.0f;
    return 1.0f; 
}


void initSingleAsteroid(std::vector<Gameobject> &gameObjects, int windowWidth, int windowHeight, AsteroidSizeType sizeType)
{
    int asteroidMinVel = 0;
    int asteroidMaxVel = 100;
    float asteroidVelMulti = 0.1;
    Asteroid asteroid = Asteroid(sizeType);  
    SDL_Point randomPosition = getRandomPosition(
        windowWidth, windowHeight, asteroid, gameObjects
    );
    asteroid.midPos[0] = randomPosition.x;
    asteroid.midPos[1] = randomPosition.y;
    asteroid.velocity = {randomSign()*asteroidVelMulti*((float)(rand() % (asteroidMaxVel-asteroidMinVel) + asteroidMinVel))/10,randomSign()*asteroidVelMulti*((float)(rand() % (asteroidMaxVel-asteroidMinVel) + asteroidMinVel))/10};
    std::cout << "Asteroidgeschwidigkeit: " << asteroid.velocity[0] << ", " << asteroid.velocity[1] <<std::endl; 
    asteroids.push_back(asteroid);
    gameObjects.push_back(asteroid);
    colObjects.push_back(asteroid);
}

void initAsteroids(Gameobject ship, int windowWidth, int windowHeight)
{
    int asteroidAmountSmall = 10;
    int asteroidAmountMedium = 10;
    std::vector<Gameobject> gameObjects = {ship};
    for (int i=0; i < asteroidAmountSmall; i++)
    {
        initSingleAsteroid(gameObjects, windowWidth, windowHeight, AsteroidSizeType::Small);
    }
    for (int i=0; i < asteroidAmountMedium; i++)
    {
        initSingleAsteroid(gameObjects, windowWidth, windowHeight, AsteroidSizeType::Medium);
    }
}
