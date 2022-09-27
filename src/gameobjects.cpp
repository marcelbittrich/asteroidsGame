#include "gameobjects.hpp"

#define PI 3.14159265359

#include <valarray>


Ship::Ship(double xPos, double yPos, int width, int height)
{
    this->xPos = xPos;
    this->yPos = yPos;
    this->width = width;
    this->height = height;
    double colRadiusOffset = 0.6;
    this->col_radius = (width/2 + height/2)/2*colRadiusOffset;
    rect = getRect();
    this->midPos.x = xPos + width/2;
    this->midPos.y = yPos + height/2;
}

Ship::Ship()
{

}

Ship::~Ship()
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
    double v_angle_degree = 0; 
    double v_sum;

    v_sum = sqrt(std::pow(velocity[0],2) + std::pow(velocity[1],2));

    if (v_sum > 0)
    {
        v_angle = atan2(velocity[0],velocity[1]);
        v_sum = std::max(v_sum - 0.01, 0.0);
    }    

    v_angle_degree = v_angle*180/PI;
    //std::cout << "direction heading: " << v_angle_degree << std::endl;

    
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
}

Asteroid::Asteroid(double xPos, double yPos, int width, int height)
{
    this->xPos = xPos;
    this->yPos = yPos;
    this->width = width;
    this->height = height;
    double colRadiusOffset = 0.6;
    this->col_radius = (width/2 + height/2)/2 * colRadiusOffset;
    rect = getRect();
    this->midPos.x = xPos + width/2;
    this->midPos.y = yPos + height/2;
}

SDL_Rect Asteroid::getRect()
{
    SDL_Rect rect;
    rect.w = width;
    rect.h = height;
    rect.x = std::round(xPos);
    rect.y = std::round(yPos);
    return rect;
}


/*void initShip(int windowWidth, int windowHeight)
{

}*/




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

void initAsteroids(SDL_Rect shipRect, int windowWidth, int windowHeight)
{
    int asteroidAmount = 2;
    int asteroidSize = 50;
    double asteroidVelMulti = 0.1;
    int asteroidMinVel = 0;
    int asteroidMaxVel = 100;
    std::vector<SDL_Rect> gameObjects = {shipRect};
    for (int i=0; i < asteroidAmount; i++)
    {
        SDL_Point randomPosition = getRandomPosition(
            windowWidth, windowHeight, asteroidSize, asteroidSize, gameObjects
        );
        Asteroid asteroid = Asteroid(randomPosition.x, randomPosition.y, asteroidSize, asteroidSize);  
        asteroid.velocity = {randomSign()*asteroidVelMulti*((double)(rand() % (asteroidMaxVel-asteroidMinVel) + asteroidMinVel))/10,randomSign()*asteroidVelMulti*((double)(rand() % (asteroidMaxVel-asteroidMinVel) + asteroidMinVel))/10};
        std::cout << "Asteroidgeschwidigkeit: " << asteroid.velocity[0] << ", " << asteroid.velocity[1] <<std::endl; 
        asteroids.push_back(asteroid);
        gameObjects.push_back(asteroid.rect);
        colObjects.push_back(asteroid);
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
}




bool doesCollide(Gameobject firstObject, Gameobject secondObject)
{
    double distance;
    distance = sqrt(pow((firstObject.xPos+firstObject.rect.w/2) - (secondObject.xPos+secondObject.rect.w/2),2) + pow((firstObject.yPos+firstObject.rect.h/2) - (secondObject.yPos+secondObject.rect.h/2),2));
   
    return distance <= firstObject.col_radius + secondObject.col_radius;

    


}

void asteroidsCollide(Gameobject &firstObject, Gameobject &secondObject)
{
    
    if (doesCollide(firstObject,secondObject))
    {
        std::cout << "Asteroid Collision" << std::endl;
        
        //source: https://docplayer.org/39258364-Ein-und-zweidimensionale-stoesse-mit-computersimulation.html

        //Stossnormale
        std::vector<double> normal;
        normal.push_back(secondObject.xPos-firstObject.xPos);
        normal.push_back(secondObject.yPos-firstObject.yPos);
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


