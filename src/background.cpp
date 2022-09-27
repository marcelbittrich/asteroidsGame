#include "background.hpp"

bgPoint::bgPoint(int xPos, int yPos)
{
    this->xPos = xPos;
    this->yPos = yPos;
    originPos.x = xPos;
    originPos.y = yPos;
    renderPos.x = originPos.x;
    renderPos.y = originPos.y;
}

float pointGetBack(float distance){
    float minVelocity = 2.0;
    float steepness = 1.0;
    float getBackVelocity = steepness*pow(distance,2);
    return (minVelocity < getBackVelocity) ? getBackVelocity : minVelocity;
}

void bgPoint::update(std::vector<Gameobject>colObjects)
{
    //get biggest col radius
    int maxColRadius = 0;
    for (auto object : colObjects){
        if(object.col_radius>maxColRadius)
        maxColRadius = object.col_radius; 
    }
    //check for nearest object
    
    for (auto object : colObjects)
    {   
        bool doesCollide = false;
        float distance;
        if (object.midPos.x-renderPos.x <= maxColRadius && object.midPos.y-renderPos.y <= maxColRadius)
        {   
            distance = sqrt(pow((object.midPos.x) - (renderPos.x),2) + pow((object.midPos.y) - (renderPos.y),2));
            if (distance <= object.col_radius)
            {
                doesCollide = true;
                std::vector<float> vDistance;
                std::vector<float> nDistance;
                float xDist = renderPos.x - object.midPos.x;
                float yDist = renderPos.y - object.midPos.y;
                vDistance.push_back(xDist);
                vDistance.push_back(yDist);

                nDistance.push_back(vDistance[0]/sqrt(distance));
                nDistance.push_back(vDistance[1]/sqrt(distance));

                renderPos.x = nDistance[0] * object.col_radius;
                renderPos.y = nDistance[1] * object.col_radius;
            }
            
        }
        

    }
    


}

void bgPoint::render(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
    SDL_RenderDrawPoint(renderer,renderPos.x,renderPos.y);
}

background::background()
{

}

background::background(int windowWidth, int windowHeight, int divider)
{
    this->height = windowHeight;
    this->width = windowWidth;

    for (int i = 0; i < divider; i++)
    {
        int xPos = width/divider * i + width/divider/2;
        for (int j = 0; j < divider; j++)
        {
            int yPos = height/divider * j + height/divider/2;
            bgPoint newPoint(xPos,yPos); 
            pointCloud.push_back(newPoint);
        } 
    }   
}

void background::update(std::vector<Gameobject>colObjects)
{

}

void background::render(SDL_Renderer *renderer)
{
    for (bgPoint singleBgPoint : pointCloud)
    {
        singleBgPoint.render(renderer);
    }
    
}