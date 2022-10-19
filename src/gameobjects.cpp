#include "gameobjects.hpp"

#define PI 3.14159265359

#include <valarray>


int Gameobject::newId = 0;

Ship::Ship(double xPos, double yPos, int size) : Gameobject()
{
    this->xPos = xPos;
    this->yPos = yPos;
    this->size = size;
    double colRadiusOffset = 0.6;
    this->colRadius = (size/2 + size/2)/2*colRadiusOffset;
    rect = getRect();
    this->midPos[0] = xPos + size/2;
    this->midPos[1] = yPos + size/2;
    animationCounter = 0;
}

Ship::Ship() : Gameobject()
{

}


SDL_Rect Ship::getRect()
{
    SDL_Rect rect;
    rect.w = size;
    rect.h = size;
    rect.x = std::round(xPos);
    rect.y = std::round(yPos);
    return rect;
}

void Ship::update(ControlBools controlBools, int windowWidth, int windowHeight)
{

    double deltaX = 0;
    double deltaY = 0;
    double vAngle = 0;
    double vSum;

    

    vSum = sqrt(std::pow(velocity[0],2) + std::pow(velocity[1],2));

    if (vSum > 0)
    {
        vAngle = atan2(velocity[0],velocity[1]);
        vSum = std::max(vSum - 0.01, 0.0);
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

    midPos[0] += velocity[0];
    midPos[1] += velocity[1];
    // std::cout <<"Updated velocity: "<< velocity << " xPos: " << destR.x << " yPos: " << destR.y << std::endl;

    midPos = calcPosIfLeaving(midPos, 0, windowWidth, windowHeight);
    xPos = midPos[0] - size / 2;
    yPos = midPos[1] - size / 2;
    rect.x = std::round(xPos);
    rect.y = std::round(yPos);

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
    SDL_Rect srcR;
    SDL_Rect destR = getRect();
    srcR.w = 300;
    srcR.h = 300;
    srcR.x = srcR.w * animationCounter;
    srcR.y = 0;
    
    SDL_RenderCopyEx(renderer, shipTex, &srcR, &destR, shipAngle, NULL, SDL_FLIP_NONE);
}


Asteroid::Asteroid(AsteroidSizeType sizeType) : Gameobject()
{
    this->sizeType = sizeType;
    if (sizeType == AsteroidSizeType::Small) this->size = 50;
    if (sizeType == AsteroidSizeType::Medium) this->size = 100;

    double colRadiusOffset = 0.6;
    this->colRadius = size/2 * colRadiusOffset;
    rect = getRect();
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


void Asteroid::update(int windowWidth, int windowHeight)
{
    if (isVisible)
    {
        midPos[0] += velocity[0];
        midPos[1] += velocity[1];
    }
    std::vector<double> newMidPosistion = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);
    if (midPos != newMidPosistion)
    {
        isVisible = false;
    }
    midPos = newMidPosistion;
    xPos = midPos[0] - size / 2;
    yPos = midPos[1] - size / 2;
    rect.x = std::round(xPos);
    rect.y = std::round(yPos);
}




bool doesCollide(Gameobject firstObject, Gameobject secondObject)
{
    if (!firstObject.isVisible || !secondObject.isVisible) return false;
    double distance;
    distance = sqrt(pow((firstObject.xPos+firstObject.rect.w/2) - (secondObject.xPos+secondObject.rect.w/2),2) + pow((firstObject.yPos+firstObject.rect.h/2) - (secondObject.yPos+secondObject.rect.h/2),2));
   
    return distance <= firstObject.colRadius + secondObject.colRadius;
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

    vAngle = shotHeadingAngle;
    
    double colRadiusOffset = 0.3;
    colRadius = size * colRadiusOffset;

    colObjects.push_back(*this);
}

void Shot::update(int windowWidth, int windowHeight)
{
    midPos[0] += velocity[0];
    midPos[1] += velocity[1];
    midPos = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);
    xPos = midPos[0] - size / 2;
    yPos = midPos[1] - size / 2;
    rect.x = std::round(xPos);
    rect.y = std::round(yPos);
}

void Shot::render(SDL_Renderer*renderer, SDL_Texture *shotTex)
{
    SDL_RenderCopyEx(renderer, shotTex, NULL, &rect, vAngle, NULL, SDL_FLIP_NONE);
}

void shoot(Ship ship)
{   

    std::vector<double> shotVelocityVector = {0, 0};


    double shotVelocity;
    shotVelocity = 15;//ship.getMaxVelocity();

    shotVelocityVector[0] = sin(ship.shipAngle/180*PI)*shotVelocity + ship.velocity[0];
    shotVelocityVector[1] = -cos(ship.shipAngle/180*PI)*shotVelocity + ship.velocity[1];

    Shot shot = Shot(ship.midPos, shotVelocityVector, ship.shipAngle);
    shots.push_back(shot);
}

bool shotIsToOld (Shot shot){   
    Uint32 deltaTime = SDL_GetTicks() - shot.creationTime;
    Uint32 maxLifeTime = 500;
    return (maxLifeTime < deltaTime);
}

std::vector<double> calcPosIfLeaving(std::vector<double> midPos, double radius, int windowWidth, int windowHeight)
{
    std::vector<double> newMidPos = midPos;

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
