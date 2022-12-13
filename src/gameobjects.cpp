#include "gameobjects.hpp"

#define PI 3.14159265359

int GameObject::newId = 0;

SDL_Rect GameObject::getRect()
{
    SDL_Rect rect;
    rect.w = width;
    rect.h = height;
    rect.x = std::round(midPos[0] - width / 2);
    rect.y = std::round(midPos[1] - height / 2);
    return rect;
}

Ship::Ship(float midPosX, float midPosY, int size) : GameObject()
{
    this->width = size;
    this->height = size;
    float colRadiusOffset = 0.6;
    this->colRadius = (width/2 + height/2)/2*colRadiusOffset;
    this->midPos = {midPosX, midPosY};
    animationCounter = 0;
}

Ship::Ship() : GameObject()
{

}


void Ship::update(ControlBools controlBools, int windowWidth, int windowHeight, float *deltaTime)
{  
    if (isVisible)
    {
        shotCounter = std::max((shotCounter - shotDecay * *deltaTime), 0.0f);
        if (!canShoot && shotCounter <= maxShotCounter/2)   canShoot = true;
        if (shotCounter >= maxShotCounter)                  canShoot = false;
    } else
    {
        canShoot = false;
        shotCounter = maxShotCounter + 1;
        timeNotVisible += *deltaTime;
        if (timeNotVisible > 3){
            timeNotVisible = 0;
            isVisible = true;
            shotCounter = 0;
        } 
    }

    float deltaX = 0;
    float deltaY = 0;
    float vAngle = 0;
    float vSum;

    vSum = sqrt(std::pow(velocity[0],2) + std::pow(velocity[1],2));

    if (vSum > 0)
    {
        vAngle = atan2(velocity[0],velocity[1]);
        vSum = std::max(vSum - 0.01 * *deltaTime * 60, 0.0);
    }    
    
    velocity.at(0) = (sin(vAngle) * vSum);
    velocity.at(1) = (cos(vAngle) * vSum); 

    if (controlBools.giveThrust)
    {
        if (vSum <= vMax)
        {
            deltaX = (sin(shipAngle*PI/180) * thrust);
            deltaY = -(cos(shipAngle*PI/180) * thrust); 
        }
    }

    //std::cout << game.frameTime << std::endl;

    velocity.at(0) += deltaX * *deltaTime * 60;
    velocity.at(1) += deltaY * *deltaTime * 60;


    if (controlBools.isTurningRight)
    {
        shipAngle += roatatingSpeed * *deltaTime * 60;
    }

    if (controlBools.isTurningLeft)
    {
        shipAngle -= roatatingSpeed * *deltaTime * 60;
    }

    midPos[0] += velocity[0] * *deltaTime * 60;
    midPos[1] += velocity[1] * *deltaTime * 60;

    midPos = calcPosIfLeaving(midPos, 0, windowWidth, windowHeight);

    //Animation update
    if (controlBools.giveThrust)
    {
        if (SDL_GetTicks() - lastUpdated > 300){
            animationCounter++;
            animationCounter = animationCounter % 3 + 1;
            lastUpdated = SDL_GetTicks();
        }
    }else
    {
        animationCounter = 0;
    }
}

void Ship::render(SDL_Renderer*renderer, SDL_Texture *shipTex)
{
    //inner triangle
    SDL_Color black = {0, 0, 0, 255};
    SDL_Color meterColor;
    if (canShoot)
    {
        meterColor = {0, 200, 0, 255};
    } else {
        meterColor = {200, 0, 0, 255};
    }
    
    SDL_Point shipNose;
    float noseDistanceToMid = height * 0.42;
    shipNose.x = midPos[0] + SDL_sinf(shipAngle/180 * PI) * noseDistanceToMid;
    shipNose.y = midPos[1] - SDL_cosf(shipAngle/180 * PI) * noseDistanceToMid;

    float shotMeterValue = std::min(shotCounter/maxShotCounter , 1.0f);

    drawTriangle(renderer, shipNose.x, shipNose.y, (float)width * 0.5, (float)height * 0.625, shipAngle, black);
    if (shotMeterValue > 0.1f) drawTriangle(renderer, shipNose.x, shipNose.y, shotMeterValue * (float)width * 0.5, shotMeterValue * (float)height * 0.625, shipAngle, meterColor);
    

    //outer texture
    SDL_Rect srcR;
    SDL_Rect destR = getRect();
    srcR.w = 300;
    srcR.h = 300;
    srcR.x = srcR.w * animationCounter;
    srcR.y = 0;

    float timeDivider = 0.25f;
    int stepValue = timeNotVisible / timeDivider;

    if(isVisible || stepValue % 2 == 1) 
    {
        SDL_SetTextureColorMod(shipTex, 255, 255, 255);
    } 
    else
    {
        SDL_SetTextureColorMod(shipTex, 100, 100, 100);
    }

    SDL_RenderCopyEx(renderer, shipTex, &srcR, &destR, shipAngle, NULL, SDL_FLIP_NONE);

}

void Ship::respawn(SDL_Renderer *renderer){
    int windowWidth, windowHeight;
    SDL_RenderGetLogicalSize(renderer, &windowWidth, &windowHeight);
    midPos = {(float)windowWidth/2, (float)windowHeight/2};

    velocity = {0, 0};

    isVisible = false;
}

void createShot(Ship ship) {
    std::vector<float> shotVelocityVector = {0, 0};
    float shotVelocity;
    shotVelocity = 15;
    shotVelocityVector[0] = sin(ship.shipAngle/180*PI)*shotVelocity + ship.velocity[0];
    shotVelocityVector[1] = -cos(ship.shipAngle/180*PI)*shotVelocity + ship.velocity[1];

    Shot(ship.midPos[0], ship.midPos[1], shotVelocityVector, ship.shipAngle);
}

void Ship::shoot()
{   
    if(canShoot)
        {
        if (Shot::shots.empty())
        {
            if(shotCounter < maxShotCounter)
            {
                createShot(*this);
                shotCounter = shotCounter + 100;
            }
        } else 
        {    
            //auto lastShot = Shot::shots.end() - 1;
            //Shot lastShotEnt = *lastShot;
            Uint32 timeSinceLastShot = SDL_GetTicks() - Shot::shots.back().creationTime;
            if(timeSinceLastShot > 250 && shotCounter < maxShotCounter){
                createShot(*this);
                shotCounter = shotCounter + 100;            
            }
        }
    }
}

std::list<Asteroid> Asteroid::asteroids;

int Asteroid::getSize(AsteroidSizeType sizeType)
{
    int size;
    if (sizeType == AsteroidSizeType::Small) size = 50;
    if (sizeType == AsteroidSizeType::Medium) size = 100;
    return size;
}

float Asteroid::getColRadius(int size)
{
    float colRadiusOffset = 0.6;
    return size/2 * colRadiusOffset;
}


Asteroid::Asteroid(float midPosX, float midPosY, std::vector<float> velocity, AsteroidSizeType sizeType) : GameObject()
{
    this->midPos = {midPosX, midPosY};
    this->sizeType = sizeType;
    this->velocity = velocity;
    int size = getSize(sizeType);
    this->colRadius = getColRadius(size);
    this->width = size;
    this->height = size;
    asteroids.push_back(*this);
}

void Asteroid::update(int windowWidth, int windowHeight, float* deltaTime)
{
    if (isVisible)
    {
        midPos[0] += velocity[0] * *deltaTime * 60;
        midPos[1] += velocity[1] * *deltaTime * 60;
    }
    
    std::vector<float> newMidPosistion = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);
    if (midPos != newMidPosistion)
    {
        isVisible = false;
    }

    if (!isVisible)
    {
        isVisible = true;
        bool canStayVisible = true;
        for (Asteroid otherAsteroid : Asteroid::asteroids)
        {
            if (id == otherAsteroid.id) continue;
            if (doesCollide(*this, otherAsteroid))
            {
                canStayVisible = false;
                break;
            }
        }
        if (!canStayVisible)
        {
            isVisible = false;
        }
    }

    midPos = newMidPosistion;
}


void Asteroid::render(SDL_Renderer*renderer, SDL_Texture *asteroidTexSmall, SDL_Texture *asteroidTexMedium)
{
    SDL_Texture* asteroidTex = nullptr;
    if (sizeType == AsteroidSizeType::Medium) {
        asteroidTex = asteroidTexMedium;
    } else if (sizeType == AsteroidSizeType::Small) {
        asteroidTex = asteroidTexSmall;
    } else {
        throw std::runtime_error("Unknown AsteroidSizeType for rendering");
    }
    if (isVisible)
    {   
        SDL_Rect asteroidRect = getRect();
        SDL_RenderCopyEx(renderer, asteroidTex, NULL, &asteroidRect, 0.0f, NULL, SDL_FLIP_NONE);
    }
}


bool doesCollide(GameObject firstObject, GameObject secondObject)
{
    if (!firstObject.isVisible || !secondObject.isVisible) return false;
    float distance;
    distance = sqrt(pow(firstObject.midPos[0] - secondObject.midPos[0], 2) + pow(firstObject.midPos[1] - secondObject.midPos[1], 2));
   
    return distance <= firstObject.colRadius + secondObject.colRadius;
}


struct CollisionOccurrence {
    int firstObjectId;
    int secondObjectId;
    Uint32 time;
};

std::vector<CollisionOccurrence> recentCollisions = {};

bool didRecentlyCollide(GameObject firstObject, GameObject secondObject)
{
    for (auto it = recentCollisions.begin(); it != recentCollisions.end(); it ++)
    {
        if ((it->firstObjectId == firstObject.id && it->secondObjectId == secondObject.id) || (it->firstObjectId == secondObject.id && it->secondObjectId == firstObject.id)) {
            if (SDL_GetTicks() > (it->time + 500))
            {
                it->time = SDL_GetTicks();
                return false;
            } else {
                //std::cout << "Did recently collide" << std::endl;
                return true;
            }
        }
    }
    CollisionOccurrence collisionOccurrence;
    collisionOccurrence.firstObjectId = firstObject.id;
    collisionOccurrence.secondObjectId = secondObject.id;
    collisionOccurrence.time = SDL_GetTicks();
    recentCollisions.push_back(collisionOccurrence);
    return false;
}

void asteroidsCollide(GameObject &firstObject, GameObject &secondObject)
{
    if (doesCollide(firstObject, secondObject) && !didRecentlyCollide(firstObject, secondObject))
    {
        // source: https://docplayer.org/39258364-Ein-und-zweidimensionale-stoesse-mit-computersimulation.html

        // distance vector
        std::vector<float> normal;
        normal.push_back(secondObject.midPos[0]-firstObject.midPos[0]);
        normal.push_back(secondObject.midPos[1]-firstObject.midPos[1]);
        
        // angle between object 1 and normal
        float f1 = (firstObject.velocity[0] * normal[0] + firstObject.velocity[1] * normal[1]) / (normal[0] * normal[0] + normal[1] * normal[1]); 
        // parallel component for object 1
        std::vector<float> vp1 = {normal[0] * f1, normal[1] * f1};
        // vertical component for object 1
        std::vector<float> vv1 = {firstObject.velocity[0] - vp1[0], firstObject.velocity[1] - vp1[1]};

        // angle between object 2 and normal
        float f2 = (secondObject.velocity[0] * normal[0] + secondObject.velocity[1] * normal[1]) / (normal[0] * normal[0] + normal[1] * normal[1]);
        // parallel component for object 2 
        std::vector<float> vp2 = {normal[0] * f2, normal[1] * f2};
        // vertical component for object 2
        std::vector<float> vv2 = {secondObject.velocity[0] - vp2[0], secondObject.velocity[1] - vp2[1]};

        int weightObject1 = PI * firstObject.colRadius * firstObject.colRadius;
        int weightObject2 = PI * secondObject.colRadius * secondObject.colRadius;

        if (weightObject1 == weightObject2)
        {
            firstObject.velocity[0] = vv1[0] + vp2[0];
            firstObject.velocity[1] = vv1[1] + vp2[1];
            secondObject.velocity[0] = vv2[0] + vp1[0];
            secondObject.velocity[1] = vv2[1] + vp1[1];
        } else {   
            float weightFactorX = 2 * ((weightObject1 * vp1[0] + weightObject2 * vp2[0]) / (weightObject1 + weightObject2));
            float weightFactorY = 2 * ((weightObject1 * vp1[1] + weightObject2 * vp2[1]) / (weightObject1 + weightObject2));
            
            vp1[0] = weightFactorX - vp1[0];
            vp1[1] = weightFactorY - vp1[1];
            vp2[0] = weightFactorX - vp2[0];
            vp2[1] = weightFactorY - vp2[1];

            firstObject.velocity[0] = vv1[0] + vp1[0];
            firstObject.velocity[1] = vv1[1] + vp1[1];
            secondObject.velocity[0] = vv2[0] + vp2[0];
            secondObject.velocity[1] = vv2[1] + vp2[1];
        }
    }
}

void spawnAsteroid(int xPos, int yPos, std::vector<float> velocity, AsteroidSizeType sizeType, std::list<GameObject> gameobjects)
{
    GameObject collisionObject = GameObject();
    collisionObject.midPos = {(float)xPos, (float)yPos};

    

    collisionObject.colRadius = Asteroid::getColRadius(Asteroid::getSize(sizeType));

    bool isSafeToSpawn = true;
    for (auto it = gameobjects.begin(); it != gameobjects.end(); it++)
    {
        if (doesCollide(collisionObject, *it))
        {
            isSafeToSpawn = false;
            break;
        }
    }  
    if (isSafeToSpawn) 
    {
        Asteroid(
            collisionObject.midPos[0],
            collisionObject.midPos[1],
            velocity,
            sizeType
        );
    }
}

std::list<Shot> Shot::shots;

Shot::Shot(float midPosX, float midPosY, std::vector<float> velocity, float shotHeadingAngle)
{
    this->velocity = velocity;
    this->midPos = {midPosX, midPosY};

    life = 3000;
    creationTime = SDL_GetTicks();

    vAngle = shotHeadingAngle;
    
    float colRadiusOffset = 0.3;
    int size = 20;
    colRadius = size * colRadiusOffset;
    this->width = size;
    this->height = size;

    shots.push_back(*this);
}

void Shot::update(int windowWidth, int windowHeight, float *deltaTime)
{
    midPos[0] += velocity[0] * *deltaTime * 60;
    midPos[1] += velocity[1] * *deltaTime * 60;
    //midPos = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);
}

void Shot::render(SDL_Renderer*renderer, SDL_Texture *shotTex)
{
    SDL_Rect rect = getRect();
    SDL_RenderCopyEx(renderer, shotTex, NULL, &rect, vAngle, NULL, SDL_FLIP_NONE);
}

bool shotIsToOld(Shot shot){ 
    Uint32 deltaTime = SDL_GetTicks() - shot.creationTime;
    Uint32 maxLifeTime = 2000;
    return (maxLifeTime < deltaTime);
}

std::vector<float> calcPosIfLeaving(std::vector<float> midPos, float radius, int windowWidth, int windowHeight)
{
    std::vector<float> newMidPos = midPos;

    if (midPos[0] < 0 - radius) // leave to left.
    {
        newMidPos[0] = windowWidth + radius;
    
    } else if (midPos[0] > windowWidth + radius) // leave to right.
    {
        newMidPos[0] = 0 - radius;
    }

    if (midPos[1] < 0 - radius) // leave to top.
    {
        newMidPos[1] = windowHeight + radius;
    } else if (midPos[1] > windowHeight + radius) // leave to bottom.
    {
        newMidPos[1] = 0 - radius;
    }
    return newMidPos;
}


std::vector<float> normalize2DVector(std::vector<float> vector2D)
{
    SDL_assert(vector2D.size() == 2);
    float factor = 1 / SDL_sqrtf(pow(vector2D[0],2) + pow(vector2D[1],2)); 
    return {vector2D[0] * factor, vector2D[1] * factor};
}

std::vector<float> set2DVectorLength(std::vector<float> vector2D, float length)
{
    SDL_assert(vector2D.size() == 2);
    std::vector<float> normalizedVector = normalize2DVector(vector2D);
    return {normalizedVector[0]* length, normalizedVector[1] * length};
}

std::vector<float> rotate2DVector(std::vector<float> old2DVector, float angleInDegree)
{
    SDL_assert(old2DVector.size() == 2);
    std::vector<float> rotated2DVector = {0,0};
    
    float angleInRadian = angleInDegree * PI / 180;
    rotated2DVector[0] = old2DVector[0] * cosf(angleInRadian) - old2DVector[1] * sinf(angleInRadian);
    rotated2DVector[1] = old2DVector[0] * sinf(angleInRadian) + old2DVector[1] * cosf(angleInRadian);

    return rotated2DVector;
}

void handleDestruction(Asteroid destroyedAsteroid)
{
    int newAsteroidSize = Asteroid::getSize(AsteroidSizeType::Small);
    
    auto oldMidPos = destroyedAsteroid.midPos;
    std::vector<float> oldVelocity = destroyedAsteroid.velocity;

    std::vector<float> spawnDirection = rotate2DVector(oldVelocity, 90);
    spawnDirection = set2DVectorLength(spawnDirection, newAsteroidSize / 2);
    Asteroid(
        spawnDirection[0] + destroyedAsteroid.midPos[0],
        spawnDirection[1] + destroyedAsteroid.midPos[1],
        {rotate2DVector(oldVelocity, 45)[0] * 2, rotate2DVector(oldVelocity, 45)[1] * 2},
        AsteroidSizeType::Small
    );

    spawnDirection = rotate2DVector(spawnDirection, 180);
    spawnDirection = set2DVectorLength(spawnDirection, newAsteroidSize / 2);
    Asteroid(
        spawnDirection[0] + destroyedAsteroid.midPos[0],
        spawnDirection[1] + destroyedAsteroid.midPos[1],
        {rotate2DVector(oldVelocity, -45)[0] * 2, rotate2DVector(oldVelocity, -45)[1] * 2},
        AsteroidSizeType::Small
    );
}

std::list<Bomb> Bomb::bombs;
std::list<Bomb*> Bomb::pCollectedBombs;

Bomb::Bomb(int xPos, int yPos, std::vector<float> velocity)
{
    midPos[0] = xPos;
    midPos[1] = yPos;
    this-> velocity = velocity;
    creationTime = SDL_GetTicks();
    isVisible = true;

    int size = 50;
    width = size;
    height = size;

    float colRadiusOffset = 0.3;
    colRadius = size * colRadiusOffset;

    bombs.push_back(*this);
}

void Bomb::update(int windowWidth, int windowHeight, float *deltaTime, Ship *ship)
{
    if (!isCollected && !isExploding)
    {
        midPos[0] += velocity[0] * *deltaTime * 60;
        midPos[1] += velocity[1] * *deltaTime * 60;
        midPos = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);

        angle += 10 * *deltaTime; 
    } 
    if (isCollected && !isExploding)
    {
        midPos = ship->midPos;
    } 
    if (isExploding)
    {
        float explosionVelocity = 20.0f;
        float timeSinceIgnition = (SDL_GetTicks() - ignitionTime) / 1000.0f;
        colRadius += timeSinceIgnition * explosionVelocity* *deltaTime * 60;
        if (timeSinceIgnition > 1.0f)
        {
            isDead = true;
        }
    }
}

void Bomb::render(SDL_Renderer*renderer, SDL_Texture *bombTex)
{
    if (isVisible)
    {
        SDL_Rect rect = getRect();
        SDL_RenderCopyEx(renderer, bombTex, NULL, &rect, angle, NULL, SDL_FLIP_NONE); 
    }
}

void Bomb::collect()
{
    isCollected = true;
    isVisible = false;
    pCollectedBombs.push_back(this);
}

void Bomb::explode()
{
    isExploding = true;
    isVisible = true;
    ignitionTime = SDL_GetTicks();
}