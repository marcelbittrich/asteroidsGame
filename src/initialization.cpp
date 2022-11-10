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
    float colRadius,
    std::vector<GameObject> gameObjects
) {
    int maxTries = 10000;
    for (int i = 0; i < maxTries; i++) {
        int x = rand() % windowWidth;
        int y = rand() % windowHeight;
        bool success = true;
        for (const GameObject &gameObject : gameObjects)
        {
            float distance = sqrt(std::pow(gameObject.midPos[0] - x, 2) + std::pow(gameObject.midPos[1] - y, 2));

            float objectColRadius;
            (gameObject.id == 1) ? objectColRadius = 200 : objectColRadius = gameObject.colRadius;
            
            if (distance < objectColRadius + colRadius) {
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

float getRandomValue (float min, float max)
{
    float randomValue = (rand() % 1001) / 1000.0f * (max - min);
    return randomValue;
}

std::vector<float> getRandomVelocity (float minVelocity, float maxVelocity)
{
    std::vector<float> velocity = {0,0};
    velocity[0] = randomSign() * getRandomValue(minVelocity, maxVelocity); 
    velocity[1] = randomSign() * getRandomValue(minVelocity, maxVelocity);
    return velocity;
}

void initSingleAsteroid(std::vector<GameObject> &gameObjects, int windowWidth, int windowHeight, AsteroidSizeType sizeType)
{
    float asteroidMinVel = 0;
    float asteroidMaxVel = 1;
    int size = Asteroid::getSize(sizeType);
    float colRadius = Asteroid::getColRadius(size);
    SDL_Point randomPosition = getRandomPosition(
        windowWidth, windowHeight, colRadius, gameObjects
    );
    Asteroid asteroid = Asteroid(randomPosition.x, randomPosition.y, getRandomVelocity(asteroidMinVel, asteroidMaxVel), sizeType);  
}

void initAsteroids(GameObject ship, int windowWidth, int windowHeight)
{
    int asteroidAmountSmall = 1;
    int asteroidAmountMedium = 1;
    std::vector<GameObject> gameObjects = {ship};
    for (int i=0; i < asteroidAmountSmall; i++)
    {
        initSingleAsteroid(gameObjects, windowWidth, windowHeight, AsteroidSizeType::Small);
    }
    for (int i=0; i < asteroidAmountMedium; i++)
    {
        initSingleAsteroid(gameObjects, windowWidth, windowHeight, AsteroidSizeType::Medium);
    }
}
