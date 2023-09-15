#include "gameobjects.hpp"

#define PI 3.14159265359

int GameObject::newId = 0;

SDL_Rect GameObject::getRect()
{
    SDL_Rect rect;
    rect.w = width;
    rect.h = height;
    rect.x = std::round(midPos.x - width / 2);
    rect.y = std::round(midPos.y - height / 2);
    return rect;
}

Ship::Ship(int midPosX, int midPosY, int size) : GameObject()
{
    this->width = size;
    this->height = size;

    this->colRadius = size / 2 * sizeToCollisonRadiusRatio;

    this->midPos = {(float)midPosX, (float)midPosY};

    animationCounter = 0;
}

Ship::Ship() : GameObject()
{
}

void Ship::update(
    InputHandler *MyInputHandler,
    int windowWidth,
    int windowHeight,
    float deltaTime)
{
    // update shooting capability and ship visibility
    if (isVisible)
    {
        shotCounter = std::max((shotCounter - shotCounterDecay * deltaTime), 0.0f);
        if (shotCounter >= maxShotCounter)
            canShoot = false;
        if (!canShoot && shotCounter <= shipCooldown)
            canShoot = true;
    }
    else
    {
        canShoot = false;
        shotCounter = maxShotCounter + 1;
        timeNotVisible += deltaTime;
        if (timeNotVisible > respawnTime)
        {
            timeNotVisible = 0;
            isVisible = true;
            shotCounter = 0;
        }
    }

    // update position
    float velocitySum = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    float velocityAngle = atan2(velocity.x, velocity.y);

    velocitySum = std::max(velocitySum - velocityDecay * deltaTime, 0.0f);
    velocity.x = (sin(velocityAngle) * velocitySum);
    velocity.y = (cos(velocityAngle) * velocitySum);

    ControlBools CurrentControlBools = (MyInputHandler->getControlBools());
    if (CurrentControlBools.giveThrust)
    {
        if (velocitySum <= velocityMax)
        {
            float deltaVelocityX = sin(rotation * PI / 180) * thrust * deltaTime;
            float deltaVelocityY = -(cos(rotation * PI / 180)) * thrust * deltaTime;
            velocity.x += deltaVelocityX;
            velocity.y += deltaVelocityY;
        }
    }

    midPos.x += velocity.x * deltaTime;
    midPos.y += velocity.y * deltaTime;

    midPos = calcPosIfLeaving(midPos, 0, windowWidth, windowHeight);

    // update ship heading
    if (CurrentControlBools.isTurningRight)
    {
        rotation += roatatingSpeed * deltaTime;
    }

    if (CurrentControlBools.isTurningLeft)
    {
        rotation -= roatatingSpeed * deltaTime;
    }

    // animation update
    if (CurrentControlBools.giveThrust)
    {
        if (SDL_GetTicks() - timeLastUpdated > timeBetweenSprites)
        {
            animationCounter++;
            animationCounter = animationCounter % 3 + 1;
            timeLastUpdated = SDL_GetTicks();
        }
    }
    else
    {
        animationCounter = 0;
    }
}

void Ship::render(SDL_Renderer *renderer, SDL_Texture *shipTex)
{
    // inner shoot indicator triangle
    SDL_Color meterColor;
    if (canShoot)
    {
        meterColor = {0, 200, 0, 255};
    }
    else
    {
        meterColor = {200, 0, 0, 255};
    }

    // calculate triangle size
    SDL_FPoint shipNose;
    float noseDistanceToShipMid = height * 0.42;
    shipNose.x = midPos.x + SDL_sinf(rotation / 180 * PI) * noseDistanceToShipMid;
    shipNose.y = midPos.y - SDL_cosf(rotation / 180 * PI) * noseDistanceToShipMid;

    SDL_Color triangleBaseColor = {0, 0, 0, 255};
    float tringleWidth = width * 0.5f;
    float trinagleHeight = height * 0.625f;

    drawTriangle(renderer, shipNose.x, shipNose.y, tringleWidth, trinagleHeight, rotation, triangleBaseColor);

    // overlay shot meter triangle
    float shotMeterValue = std::min(shotCounter / maxShotCounter, 1.0f);
    if (shotMeterValue > 0.1f)
    {
        drawTriangle(renderer, shipNose.x, shipNose.y, shotMeterValue * tringleWidth, shotMeterValue * trinagleHeight, rotation, meterColor);
    }

    // draw ship texture
    if (!isVisible)
    {
        float timeStepSize = 0.25f;
        int stepValue = timeNotVisible / timeStepSize;
        // greyed out texture
        if (stepValue % 2 == 1)
            SDL_SetTextureColorMod(shipTex, 100, 100, 100);
        else
            SDL_SetTextureColorMod(shipTex, 255, 255, 255);
    }
    else
    {
        // default texture
        SDL_SetTextureColorMod(shipTex, 255, 255, 255);
    }

    SDL_Rect srcR;
    SDL_Rect destR = getRect();
    srcR.w = 300;
    srcR.h = 300;
    srcR.x = srcR.w * animationCounter;
    srcR.y = 0;

    SDL_RenderCopyEx(renderer, shipTex, &srcR, &destR, rotation, NULL, SDL_FLIP_NONE);
}

void Ship::reset(SDL_Renderer *renderer)
{
    int windowWidth, windowHeight;
    SDL_RenderGetLogicalSize(renderer, &windowWidth, &windowHeight);

    midPos = {windowWidth / 2.0f, windowHeight / 2.0f};
    velocity = {0.0f, 0.0f};
    rotation = 0.f;
}

void Ship::respawn(SDL_Renderer *renderer)
{
    reset(renderer);
    isVisible = false;
}

void Ship::createShot()
{
    SDL_FPoint shotVelocityVector = {0, 0};

    shotVelocityVector.x = sin(rotation / 180 * PI) * shotVelocity + velocity.x;
    shotVelocityVector.y = -cos(rotation / 180 * PI) * shotVelocity + velocity.y;

    Shot(midPos.x, midPos.y, shotVelocityVector, rotation);
}

void Ship::shoot()
{
    if (canShoot)
    {
        if (Shot::shots.empty())
        {
            if (shotCounter < maxShotCounter)
            {
                createShot();
                shotCounter = shotCounter + 100;
            }
        }
        else
        {
            // auto lastShot = Shot::shots.end() - 1;
            // Shot lastShotEnt = *lastShot;
            Uint32 timeSinceLastShot = SDL_GetTicks() - Shot::shots.back().creationTime;
            if (timeSinceLastShot > 250 && shotCounter < maxShotCounter)
            {
                createShot();
                shotCounter = shotCounter + 100;
            }
        }
    }
}

std::list<Asteroid> Asteroid::asteroids;

int Asteroid::getSize(AsteroidSizeType sizeType)
{
    int size;
    if (sizeType == AsteroidSizeType::Small)
        size = 50;
    if (sizeType == AsteroidSizeType::Medium)
        size = 100;
    return size;
}

float Asteroid::getColRadius(int size)
{
    float colRadiusOffset = 0.6;
    return size / 2 * colRadiusOffset;
}

Asteroid::Asteroid(float midPosX, float midPosY, SDL_FPoint velocity, AsteroidSizeType sizeType) : GameObject()
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

void Asteroid::update(int windowWidth, int windowHeight, float deltaTime)
{
    if (isVisible)
    {
        midPos.x += velocity.x * deltaTime * 60;
        midPos.y += velocity.y * deltaTime * 60;
    }

    SDL_FPoint newMidPosistion = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);

    if ((midPos.x != newMidPosistion.x) || (midPos.y != newMidPosistion.y))
    {
        isVisible = false;
    }

    if (!isVisible)
    {
        isVisible = true;
        bool canStayVisible = true;
        for (Asteroid otherAsteroid : Asteroid::asteroids)
        {
            if (id == otherAsteroid.id)
                continue;
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

void Asteroid::render(SDL_Renderer *renderer, SDL_Texture *asteroidTexSmall, SDL_Texture *asteroidTexMedium)
{
    SDL_Texture *asteroidTex = nullptr;
    if (sizeType == AsteroidSizeType::Medium)
    {
        asteroidTex = asteroidTexMedium;
    }
    else if (sizeType == AsteroidSizeType::Small)
    {
        asteroidTex = asteroidTexSmall;
    }
    else
    {
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
    if (!firstObject.isVisible || !secondObject.isVisible)
        return false;
    float distance;
    distance = sqrt(pow(firstObject.midPos.x - secondObject.midPos.x, 2) + pow(firstObject.midPos.y - secondObject.midPos.y, 2));

    return distance <= firstObject.colRadius + secondObject.colRadius;
}

struct CollisionOccurrence
{
    int firstObjectId;
    int secondObjectId;
    Uint32 time;
};

std::vector<CollisionOccurrence> recentCollisions = {};

bool didRecentlyCollide(GameObject firstObject, GameObject secondObject)
{
    for (auto it = recentCollisions.begin(); it != recentCollisions.end(); it++)
    {
        if ((it->firstObjectId == firstObject.id && it->secondObjectId == secondObject.id) || (it->firstObjectId == secondObject.id && it->secondObjectId == firstObject.id))
        {
            if (SDL_GetTicks() > (it->time + 500))
            {
                it->time = SDL_GetTicks();
                return false;
            }
            else
            {
                // std::cout << "Did recently collide" << std::endl;
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
        normal.push_back(secondObject.midPos.x - firstObject.midPos.x);
        normal.push_back(secondObject.midPos.y - firstObject.midPos.y);

        // angle between object 1 and normal
        float f1 = (firstObject.velocity.x * normal[0] + firstObject.velocity.y * normal[1]) / (normal[0] * normal[0] + normal[1] * normal[1]);
        // parallel component for object 1
        std::vector<float> vp1 = {normal[0] * f1, normal[1] * f1};
        // vertical component for object 1
        std::vector<float> vv1 = {firstObject.velocity.x - vp1[0], firstObject.velocity.y - vp1[1]};

        // angle between object 2 and normal
        float f2 = (secondObject.velocity.x * normal[0] + secondObject.velocity.y * normal[1]) / (normal[0] * normal[0] + normal[1] * normal[1]);
        // parallel component for object 2
        std::vector<float> vp2 = {normal[0] * f2, normal[1] * f2};
        // vertical component for object 2
        std::vector<float> vv2 = {secondObject.velocity.x - vp2[0], secondObject.velocity.y - vp2[1]};

        int weightObject1 = PI * firstObject.colRadius * firstObject.colRadius;
        int weightObject2 = PI * secondObject.colRadius * secondObject.colRadius;

        if (weightObject1 == weightObject2)
        {
            firstObject.velocity.x = vv1[0] + vp2[0];
            firstObject.velocity.y = vv1[1] + vp2[1];
            secondObject.velocity.x = vv2[0] + vp1[0];
            secondObject.velocity.y = vv2[1] + vp1[1];
        }
        else
        {
            float weightFactorX = 2 * ((weightObject1 * vp1[0] + weightObject2 * vp2[0]) / (weightObject1 + weightObject2));
            float weightFactorY = 2 * ((weightObject1 * vp1[1] + weightObject2 * vp2[1]) / (weightObject1 + weightObject2));

            vp1[0] = weightFactorX - vp1[0];
            vp1[1] = weightFactorY - vp1[1];
            vp2[0] = weightFactorX - vp2[0];
            vp2[1] = weightFactorY - vp2[1];

            firstObject.velocity.x = vv1[0] + vp1[0];
            firstObject.velocity.y = vv1[1] + vp1[1];
            secondObject.velocity.x = vv2[0] + vp2[0];
            secondObject.velocity.y = vv2[1] + vp2[1];
        }
    }
}

void spawnAsteroid(int xPos, int yPos, SDL_FPoint velocity, AsteroidSizeType sizeType, std::list<GameObject> gameobjects)
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
            collisionObject.midPos.x,
            collisionObject.midPos.y,
            velocity,
            sizeType);
    }
}

std::list<Shot> Shot::shots;

Shot::Shot(float midPosX, float midPosY, SDL_FPoint velocity, float shotHeadingAngle)
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

void Shot::update(int windowWidth, int windowHeight, float deltaTime)
{
    midPos.x += velocity.x * deltaTime;
    midPos.y += velocity.y * deltaTime;
    // midPos = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);
}

void Shot::render(SDL_Renderer *renderer, SDL_Texture *shotTex)
{
    SDL_Rect rect = getRect();
    SDL_RenderCopyEx(renderer, shotTex, NULL, &rect, vAngle, NULL, SDL_FLIP_NONE);
}

bool shotIsToOld(Shot shot)
{
    Uint32 deltaTime = SDL_GetTicks() - shot.creationTime;
    Uint32 maxLifeTime = 2000;
    return (maxLifeTime < deltaTime);
}

SDL_FPoint calcPosIfLeaving(SDL_FPoint midPos, float radius, int windowWidth, int windowHeight)
{
    SDL_FPoint newMidPos = midPos;

    if (midPos.x < 0 - radius) // leave to left.
    {
        newMidPos.x = windowWidth + radius;
    }
    else if (midPos.x > windowWidth + radius) // leave to right.
    {
        newMidPos.x = 0 - radius;
    }

    if (midPos.y < 0 - radius) // leave to top.
    {
        newMidPos.y = windowHeight + radius;
    }
    else if (midPos.y > windowHeight + radius) // leave to bottom.
    {
        newMidPos.y = 0 - radius;
    }
    return newMidPos;
}

SDL_FPoint normalize2DVector(SDL_FPoint vector2D)
{
    float factor = 1 / SDL_sqrtf(pow(vector2D.x, 2) + pow(vector2D.y, 2));
    return {vector2D.x * factor, vector2D.y * factor};
}

SDL_FPoint set2DVectorLength(SDL_FPoint vector2D, float length)
{
    SDL_FPoint normalizedVector = normalize2DVector(vector2D);
    return {normalizedVector.x * length, normalizedVector.y * length};
}

SDL_FPoint rotate2DVector(SDL_FPoint old2DVector, float angleInDegree)
{
    SDL_FPoint rotated2DVector = {0, 0};

    float angleInRadian = angleInDegree * PI / 180;
    rotated2DVector.x = old2DVector.x * cosf(angleInRadian) - old2DVector.x * sinf(angleInRadian);
    rotated2DVector.y = old2DVector.y * sinf(angleInRadian) + old2DVector.y * cosf(angleInRadian);

    return rotated2DVector;
}

void handleDestruction(Asteroid destroyedAsteroid)
{
    int newAsteroidSize = Asteroid::getSize(AsteroidSizeType::Small);

    SDL_FPoint oldMidPos = destroyedAsteroid.midPos;
    SDL_FPoint oldVelocity = destroyedAsteroid.velocity;

    SDL_FPoint spawnDirection = rotate2DVector(oldVelocity, 90);
    spawnDirection = set2DVectorLength(spawnDirection, newAsteroidSize / 2);
    Asteroid(
        spawnDirection.x + destroyedAsteroid.midPos.x,
        spawnDirection.y + destroyedAsteroid.midPos.y,
        {rotate2DVector(oldVelocity, 45).x * 2, rotate2DVector(oldVelocity, 45).y * 2},
        AsteroidSizeType::Small);

    spawnDirection = rotate2DVector(spawnDirection, 180);
    spawnDirection = set2DVectorLength(spawnDirection, newAsteroidSize / 2);
    Asteroid(
        spawnDirection.x + destroyedAsteroid.midPos.x,
        spawnDirection.y + destroyedAsteroid.midPos.y,
        {rotate2DVector(oldVelocity, -45).x * 2, rotate2DVector(oldVelocity, -45).y * 2},
        AsteroidSizeType::Small);
}

std::list<Bomb> Bomb::bombs;
std::list<Bomb *> Bomb::pCollectedBombs;

Bomb::Bomb(int xPos, int yPos, SDL_FPoint velocity)
{
    midPos.x = xPos;
    midPos.y = yPos;
    this->velocity = velocity;
    creationTime = SDL_GetTicks();
    isVisible = true;

    int size = 50;
    width = size;
    height = size;

    float colRadiusOffset = 0.3;
    colRadius = size * colRadiusOffset;

    bombs.push_back(*this);
}

void Bomb::update(int windowWidth, int windowHeight, float deltaTime, Ship *ship)
{
    if (!isCollected && !isExploding)
    {
        midPos.x += velocity.x * deltaTime * 60;
        midPos.y += velocity.y * deltaTime * 60;
        midPos = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);

        angle += 10 * deltaTime;
    }
    if (isCollected && !isExploding)
    {
        midPos = ship->midPos;
    }
    if (isExploding)
    {
        float explosionVelocity = 20.0f;
        float timeSinceIgnition = (SDL_GetTicks() - ignitionTime) / 1000.0f;
        colRadius += timeSinceIgnition * explosionVelocity * deltaTime * 60;
        if (timeSinceIgnition > 1.0f)
        {
            isDead = true;
        }
    }
}

void Bomb::render(SDL_Renderer *renderer, SDL_Texture *bombTex)
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