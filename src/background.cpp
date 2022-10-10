#include "background.hpp"

bgPoint::bgPoint()
{

}

bgPoint::bgPoint(int xPos, int yPos)
{
    this->xPos = xPos;
    this->yPos = yPos;
    originPos.x = xPos;
    originPos.y = yPos;
    renderPos.x = originPos.x;
    renderPos.y = originPos.y;
    color[0] = rand() %255;
    color[1] = rand() %255;
    color[2] = rand() %255;
    color[3] = 255;
    onOrigin = true;
}

void bgPoint::returnToOrigin(){
    float minVelocity = 0.5;
    float steepness = 0.005;
    float distance; 
    distance = sqrt(pow((originPos.x-xPos),2) + pow((originPos.y-yPos),2));

    std::vector<float> vDistance;
    std::vector<float> nDistance;
    std::vector<float> vChange;
    vDistance.push_back(originPos.x-xPos);
    vDistance.push_back(originPos.y-yPos);
    nDistance.push_back(vDistance[0]/(distance));
    nDistance.push_back(vDistance[1]/(distance));
    float distanceDependentVelocity = steepness*pow(distance,2);

    float getBackVelocity;
    getBackVelocity = std::max(minVelocity,distanceDependentVelocity);

    vChange.push_back(nDistance[0] * getBackVelocity);
    vChange.push_back(nDistance[1] * getBackVelocity);

    if (sqrt(pow(vChange[0],2) + pow(vChange[1],2))>distance)
    {
        xPos = originPos.x; 
        yPos = originPos.y;
        renderPos.x = originPos.x;
        renderPos.y = originPos.y;
        onOrigin = true;
    }   else {
        xPos += vChange[0];
        yPos += vChange[1];
        renderPos.x = round(xPos);
        renderPos.y = round(yPos);
        onOrigin = false;
    }
}

void bgPoint::update(Gameobject colObject)
{   
    float distance; 
    distance = sqrt(pow((colObject.midPos[0]) - (xPos),2) + pow((colObject.midPos[1]) - (yPos),2));
    if (distance <= colObject.col_radius)
    {
        std::vector<float> vDistance;
        std::vector<float> nDistance;
        float xDist = xPos - colObject.midPos[0];
        float yDist = yPos - colObject.midPos[1];
        vDistance.push_back(xDist);
        vDistance.push_back(yDist);

        nDistance.push_back(vDistance[0]/(distance));
        nDistance.push_back(vDistance[1]/(distance));

        xPos = colObject.midPos[0] + nDistance[0] * colObject.col_radius;
        yPos = colObject.midPos[1] + nDistance[1] * colObject.col_radius;

        renderPos.x = round(xPos);
        renderPos.y = round(yPos);
        onOrigin = false;
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
    this->divider = divider;

    for (int i = 0; i < divider; i++)
    {
        int xPos = width/divider * i + width/divider/2;
        std::vector<bgPoint> bgPointColumn;
        for (int j = 0; j < divider; j++)
        {
            int yPos = height/divider * j + height/divider/2;

            bgPoint newPoint(xPos,yPos); 
            bgPointColumn.push_back(newPoint);
        }
        pointCloud.push_back(bgPointColumn);
    }  
}

void background::update(std::vector<Gameobject>colObjects)
{   
    for (auto object:colObjects)
    {
        //Size of grid divisions
        int divX = width/divider;
        int divY = height/divider;
        //Grid position of colObject
        int gridPosX = object.midPos[0]/divX;
        int gridPosY = object.midPos[1]/divY;
        //Size of detection box
        int w = (int)(object.col_radius/divX);
        int h = (int)(object.col_radius/divY);

        for (int i = gridPosX-w; i <= gridPosX+w; i++)
        {
            for (int j = gridPosY-h; j <= gridPosY+h; j++)
            {
                if (i >= 0 && i <= divider-1 && j >= 0 && j <= divider-1){
                    pointCloud[i][j].update(object);
                }
            }
        }
    }
    for (std::vector<bgPoint> &bgPointColumn : pointCloud)
    {
        for (bgPoint &singleBgPoint : bgPointColumn)
            if (!singleBgPoint.onOrigin)
            {   
                singleBgPoint.returnToOrigin();
            }    
    }
}


void background::render(SDL_Renderer *renderer)
{
    for (std::vector<bgPoint> bgPointColumn : pointCloud)
    {
        for (bgPoint singleBgPoint : bgPointColumn)
            if (!singleBgPoint.onOrigin)
            {   
                singleBgPoint.render(renderer);
            }    
    }
}


