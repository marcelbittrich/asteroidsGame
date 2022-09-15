#include "gameobjects.hpp"

#define PI 3.14159265359



Ship::Ship(double xPos, double yPos, int width, int height)
{
    this->xPos = xPos;
    this->yPos = yPos;
    this->width = width;
    this->height = height;
    this->col_radius = (width/2 + height/2)/2;
    rect = getRect();
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



    if (xPos < 0){
        xPos = windowWidth;
    }
    if (xPos > windowWidth){
        xPos = 0;
    }
    if (yPos < 0){
        yPos = windowHeight;
    }
    if (yPos > windowHeight){
        yPos = 0;
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
    this->col_radius = (width/2 + height/2)/2;
    rect = getRect();
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


std::vector<Asteroid> asteroids = {};

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

void initAsteroids(SDL_Rect shipRect, int windowWidth, int windowHeight)
{
    int asteroidAmount = 20;
    int asteroidSize = 50;
    std::vector<SDL_Rect> gameObjects = {shipRect};
    for (int i=0; i < asteroidAmount; i++)
    {
        SDL_Point randomPosition = getRandomPosition(
            windowWidth, windowHeight, asteroidSize, asteroidSize, gameObjects
        );
        Asteroid asteroid = Asteroid(randomPosition.x, randomPosition.y, asteroidSize, asteroidSize);
        asteroids.push_back(asteroid);
        gameObjects.push_back(asteroid.rect);
    }
}

bool doesCollide(Gameobject firstObject, Gameobject secondObject)
{
    double distance;
    distance = sqrt(pow(firstObject.xPos - secondObject.xPos,2) + pow(firstObject.yPos - secondObject.yPos,2));
   
    return distance <= firstObject.col_radius + secondObject.col_radius;
}
