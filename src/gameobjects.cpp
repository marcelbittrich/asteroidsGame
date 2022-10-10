#include "gameobjects.hpp"

#define PI 3.14159265359

#include <valarray>


int Gameobject::newId = 0;

Ship::Ship(double xPos, double yPos, int width, int height) : Gameobject()
{
    this->xPos = xPos;
    this->yPos = yPos;
    this->width = width;
    this->height = height;
    double colRadiusOffset = 0.6;
    this->col_radius = (width/2 + height/2)/2*colRadiusOffset;
    rect = getRect();
    this->midPos[0] = xPos + width/2;
    this->midPos[1] = yPos + height/2;
}

Ship::Ship() : Gameobject()
{

}


SDL_Rect Ship::getRect()
{
    SDL_Rect rect;
    rect.w = width;
    rect.h = height;
    rect.x = std::round(xPos);
    rect.y = std::round(yPos);
    return rect;
}

void Ship::update(ControlBools controlBools, int windowWidth, int windowHeight)
{

    double deltaX = 0;
    double deltaY = 0;
    double v_angle = 0;
    double v_sum;

    

    v_sum = sqrt(std::pow(velocity[0],2) + std::pow(velocity[1],2));

    if (v_sum > 0)
    {
        v_angle = atan2(velocity[0],velocity[1]);
        v_sum = std::max(v_sum - 0.01, 0.0);
    }    

    
    velocity.at(0) = (sin(v_angle) * v_sum);
    velocity.at(1) = (cos(v_angle) * v_sum); 

    if (controlBools.giveThrust)
    {


        if (v_sum <= v_max)
        {
            deltaX = (sin(shipAngle*PI/180) * thrust);
            deltaY = -(cos(shipAngle*PI/180) * thrust); 
        }
        
    }

    velocity.at(0) += deltaX;
    velocity.at(1) += deltaY;


    if (controlBools.isTurningRight)
    {
        shipAngle += roatatingSpeed;
    }

    if (controlBools.isTurningLeft)
    {
        shipAngle -= roatatingSpeed;
    }

    xPos += velocity.at(0);
    yPos += velocity.at(1);
    // std::cout <<"Updated velocity: "<< velocity << " xPos: " << destR.x << " yPos: " << destR.y << std::endl;

    if (xPos < 0 - rect.w){
        xPos = windowWidth + rect.w;
    }
    if (xPos > windowWidth + rect.w){
        xPos = 0 - rect.w;
    }
    if (yPos < 0 - rect.h){
        yPos = windowHeight + rect.h;
    }
    if (yPos > windowHeight + rect.h){
        yPos = 0 - rect.h;
    }

    rect.x = std::round(xPos);
    rect.y = std::round(yPos);
    midPos[0] = xPos + width/2;
    midPos[1] = yPos + height/2;

    

}

Asteroid::Asteroid(double xPos, double yPos, int size) : Gameobject()
{
    this->xPos = xPos;
    this->yPos = yPos;
    this->size = size;
    double colRadiusOffset = 0.6;
    this->col_radius = size/2 * colRadiusOffset;
    rect = getRect();
    this->midPos[0] = xPos + size/2;
    this->midPos[1] = yPos + size/2;
}

SDL_Rect Asteroid::getRect()
{
    SDL_Rect rect;
    rect.w = size;
    rect.h = size;
    rect.x = std::round(xPos);
    rect.y = std::round(yPos);
    return rect;
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


void Asteroid::update(int windowWidth, int windowHeight)
{
    xPos += velocity[0];
    yPos += velocity[1];

    if (xPos < 0 - rect.w){
        xPos = windowWidth + rect.w;
    }
    if (xPos > windowWidth + rect.w){
        xPos = 0 - rect.w;
    }
    if (yPos < 0 - rect.h){
        yPos = windowHeight + rect.h;
    }
    if (yPos > windowHeight + rect.h){
        yPos = 0 - rect.h;
    }

    rect.x = std::round(xPos);
    rect.y = std::round(yPos);
    midPos[0] = xPos + size/2;
    midPos[1] = yPos + size/2;
}




bool doesCollide(Gameobject firstObject, Gameobject secondObject)
{
    double distance;
    distance = sqrt(pow((firstObject.xPos+firstObject.rect.w/2) - (secondObject.xPos+secondObject.rect.w/2),2) + pow((firstObject.yPos+firstObject.rect.h/2) - (secondObject.yPos+secondObject.rect.h/2),2));
   
    return distance <= firstObject.col_radius + secondObject.col_radius;
}


struct CollisionOccurrence {
    int firstObjectId;
    int secondObjectId;
    Uint32 time;
};

std::vector<CollisionOccurrence> recentCollisions = {};

bool didRecentlyCollide(Gameobject firstObject, Gameobject secondObject)
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

void asteroidsCollide(Gameobject &firstObject, Gameobject &secondObject)
{
    if (doesCollide(firstObject,secondObject) && !didRecentlyCollide(firstObject, secondObject))
    {
        // std::cout << "Asteroid Collision" << std::endl;
        
        //source: https://docplayer.org/39258364-Ein-und-zweidimensionale-stoesse-mit-computersimulation.html

        //Stossnormale
        std::vector<double> normal;
        normal.push_back(secondObject.midPos[0]-firstObject.midPos[0]);
        normal.push_back(secondObject.midPos[1]-firstObject.midPos[1]);
        //angle between object 1 and normal
        std::valarray<double>n(normal.size());
        std::copy(begin(normal), end(normal), begin(n));

        std::valarray<double>v1(firstObject.velocity.size());
        std::copy(begin(firstObject.velocity), end(firstObject.velocity), begin(v1));


        float f1 = (v1[0]*n[0]+v1[1]*n[1])/(n[0]*n[0]+n[1]*n[1]); 
        //parallel component for object 1
        std::valarray<double> vp1 = n * f1;  
        //vertical component for object 1
        std::valarray<double> vv1 = v1 - vp1;

        std::valarray<double>v2(secondObject.velocity.size());
        std::copy(begin(secondObject.velocity), end(secondObject.velocity), begin(v2));


        double f2 = (v2[0]*n[0]+v2[1]*n[1])/(n[0]*n[0]+n[1]*n[1]); 
        //parallel component for object 2
        std::valarray<double> vp2 = n * f2;  
        //vertical component for object 2
        std::valarray<double> vv2 = v2 - vp2;

        //if both objects are equal in weight
        v1 = vv1 + vp2;
        v2 = vv2 + vp1;

        firstObject.velocity[0] = v1[0];
        firstObject.velocity[1] = v1[1];
        secondObject.velocity[0] = v2[0];
        secondObject.velocity[1] = v2[1];
    }
}

SDL_Rect Shot::getRect()
{
    SDL_Rect rect;
    rect.w = size;
    rect.h = size;
    rect.x = std::round(xPos);
    rect.y = std::round(yPos);
    return rect;
}

Shot::Shot(std::vector<double> midPos, std::vector<double> velocity, double shotHeadingAngle)
{
    this->velocity = velocity;
    this->midPos[0]=midPos[0];
    this->midPos[1]=midPos[1];

    size = 20;
    xPos = midPos[0] - size/2;
    yPos = midPos[1] - size/2;

    this->rect = getRect();
    life = 3000;
    creationTime = SDL_GetTicks();

    v_angle = shotHeadingAngle;
    
    double colRadiusOffset = 0.6;
    col_radius = size * colRadiusOffset;

    colObjects.push_back(*this);
}

void Shot::update(int windowWidth, int windowHeight)
{
    xPos += velocity[0];
    yPos += velocity[1];

    if (xPos < 0 - rect.w){
        xPos = windowWidth + rect.w;
    }
    if (xPos > windowWidth + rect.w){
        xPos = 0 - rect.w;
    }
    if (yPos < 0 - rect.h){
        yPos = windowHeight + rect.h;
    }
    if (yPos > windowHeight + rect.h){
        yPos = 0 - rect.h;
    }

    rect.x = std::round(xPos);
    rect.y = std::round(yPos);
    midPos[0] = xPos + size/2;
    midPos[1] = yPos + size/2;
}

void Shot::render(SDL_Renderer*renderer, SDL_Texture *shotTex)
{
    SDL_RenderCopyEx(renderer, shotTex, NULL, &rect, v_angle, NULL, SDL_FLIP_NONE);
}

void shoot(Ship ship)
{   

    std::vector<double> shotVelocityVector = {0, 0};


    double shotVelocity;
    shotVelocity = ship.getMaxVelocity();

    shotVelocityVector[0] = sin(ship.shipAngle/180*PI)*shotVelocity;
    shotVelocityVector[1] = -cos(ship.shipAngle/180*PI)*shotVelocity;

    Shot shot = Shot(ship.midPos, shotVelocityVector, ship.shipAngle);
    shots.push_back(shot);
}

bool shotIsToOld (Shot shot){   
    Uint32 deltaTime = SDL_GetTicks() - shot.creationTime;
    Uint32 maxLifeTime = 500;
    return (maxLifeTime < deltaTime);
}

