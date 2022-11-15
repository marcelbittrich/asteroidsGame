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

void bgPoint::returnToOrigin(float *deltaTime){
    float minVelocity = 0.5;
    float steepness = 0.005;
    float squareDistance, distance; 
    squareDistance = (originPos.x-xPos) * (originPos.x-xPos) + (originPos.y-yPos) * (originPos.y-yPos);
    distance = SDL_sqrtf(squareDistance);

    vDistance[0] = (originPos.x-xPos);
    vDistance[1] = (originPos.y-yPos);
    nDistance[0] = (vDistance[0]/(distance));
    nDistance[1] = (vDistance[1]/(distance));
    float distanceDependentVelocity = steepness * distance * distance;

    float getBackVelocity;
    getBackVelocity = std::max(minVelocity,distanceDependentVelocity) * *deltaTime * 60;

    vChange[0] = (nDistance[0] * getBackVelocity);
    vChange[1] = (nDistance[1] * getBackVelocity);
    
    if (vChange[0] * vChange[0] + vChange[1] * vChange[1] > squareDistance)
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

void bgPoint::update(GameObject colObject)
{   
    squareDistance = (colObject.midPos[0] - xPos) * (colObject.midPos[0] - xPos) + (colObject.midPos[1] - yPos) * (colObject.midPos[1] - yPos);
    squareColRadius = colObject.colRadius * colObject.colRadius;
    if (squareDistance <= squareColRadius)
    {
        float xDist = xPos - colObject.midPos[0];
        float yDist = yPos - colObject.midPos[1];
        vDistance[0] = xDist;
        vDistance[1] = yDist;

        float distance = SDL_sqrtf(squareDistance);

        nDistance[0] = vDistance[0]/distance;
        nDistance[1] = vDistance[1]/distance;

        xPos = colObject.midPos[0] + nDistance[0] * colObject.colRadius;
        yPos = colObject.midPos[1] + nDistance[1] * colObject.colRadius;

        renderPos.x = round(xPos);
        renderPos.y = round(yPos);
        onOrigin = false;
    }
}

void bgPoint::render(SDL_Renderer *renderer)
{
    float pointSizeScale = 1.5f;
    int logicWidth = 0;
    int logicHeight = 0;
    SDL_RenderGetLogicalSize(renderer, &logicWidth, &logicHeight);
    SDL_RenderSetLogicalSize(renderer, round(logicWidth/pointSizeScale), round(logicHeight/pointSizeScale));
    int scaledPosX = round(renderPos.x/pointSizeScale); 
    int scaledPosY = round(renderPos.y/pointSizeScale); 
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
    SDL_RenderDrawPoint(renderer,scaledPosX,scaledPosY);
    SDL_RenderSetLogicalSize(renderer, logicWidth, logicHeight);
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
        int xPos = width/(float)divider * i + width/(float)divider/2.0f;
        std::vector<bgPoint> bgPointColumn;
        for (int j = 0; j < divider; j++)
        {
            int yPos = height/(float)divider * j + height/(float)divider/2.0f;
            bgPoint newPoint(xPos,yPos); 
            bgPointColumn.push_back(newPoint);
        }
        pointCloud.push_back(bgPointColumn);
    }  
}

void background::update(std::list<GameObject>colObjects, float *updateTime)
{       
    for (const GameObject &object : colObjects)
    {
        //Size of grid divisions
        float divX = width/(float)divider;
        float divY = height/(float)divider;
        //Grid position of colObject
        int gridPosX = object.midPos[0]/divX;
        int gridPosY = object.midPos[1]/divY;
        //Size of detection box
        int w = (int)(object.colRadius/divX);
        int h = (int)(object.colRadius/divY);

        for (int i = gridPosX-w; i <= gridPosX+w; i++)
        {
            if (i >= 0 && i <= divider-1)
            {
                for (int j = gridPosY-h; j <= gridPosY+h; j++)
                {
                    if (j >= 0 && j <= divider-1){
                        pointCloud[i][j].update(object);                  
                    }
                }
            }
        }
    }

    for (std::vector<bgPoint> &bgPointColumn : pointCloud)
    {
        for (bgPoint &singleBgPoint : bgPointColumn)
            if (!singleBgPoint.onOrigin)
            {   
                singleBgPoint.returnToOrigin(updateTime);
            }    
    }
}


void background::render(SDL_Renderer *renderer)
{
    for (std::vector<bgPoint> &bgPointColumn : pointCloud)
    {
        for (bgPoint &singleBgPoint : bgPointColumn)
            if (!singleBgPoint.onOrigin)
            {   
                singleBgPoint.render(renderer);
            }    
    }
}


