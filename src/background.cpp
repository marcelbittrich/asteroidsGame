#include "background.hpp"

backgroundPoint::backgroundPoint()
{

}

backgroundPoint::backgroundPoint(int xPos, int yPos)
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

void backgroundPoint::returnToOrigin(float *deltaTime)
{
    squareDistanceToOrigin = (originPos.x-xPos) * (originPos.x-xPos) + (originPos.y-yPos) * (originPos.y-yPos);
    distanceToOrigin = SDL_sqrtf(squareDistanceToOrigin);

    vectorDistance[0] = (originPos.x-xPos);
    vectorDistance[1] = (originPos.y-yPos);
    normalizedDistance[0] = (vectorDistance[0]/(distanceToOrigin));
    normalizedDistance[1] = (vectorDistance[1]/(distanceToOrigin));

    float distanceDependentVelocity = distanceVelocityFunctionSteepness * distanceToOrigin * distanceToOrigin;

    float returnToOriginVelocity;
    returnToOriginVelocity = std::max(minReturnVelocity,distanceDependentVelocity) * *deltaTime * 60;

    vectorChange[0] = (normalizedDistance[0] * returnToOriginVelocity);
    vectorChange[1] = (normalizedDistance[1] * returnToOriginVelocity);
    
    //if point will be cloase to origin, put it on origin
    if (vectorChange[0] * vectorChange[0] + vectorChange[1] * vectorChange[1] > squareDistanceToOrigin)
    {
        xPos = originPos.x; 
        yPos = originPos.y;

        renderPos.x = originPos.x;
        renderPos.y = originPos.y;

        onOrigin = true;       
    }   
    else 
    {
        xPos += vectorChange[0];
        yPos += vectorChange[1];

        renderPos.x = round(xPos);
        renderPos.y = round(yPos);

        onOrigin = false;
    }
}

void backgroundPoint::update(GameObject colObject)
{   
    //AABB collision check
    if (xPos > colObject.midPos[0] - colObject.colRadius 
    && xPos < colObject.midPos[0] + colObject.colRadius
    && yPos > colObject.midPos[1] - colObject.colRadius
    && yPos < colObject.midPos[1] + colObject.colRadius)
    {
        //distance based collision check
        float squareDistanceToObject = (colObject.midPos[0] - xPos) * (colObject.midPos[0] - xPos) 
                                    + (colObject.midPos[1] - yPos) * (colObject.midPos[1] - yPos);
        float squareObjectCollisonRadius = colObject.colRadius * colObject.colRadius;

        if (squareDistanceToObject <= squareObjectCollisonRadius)
        {
            float distanceToObject = SDL_sqrtf(squareDistanceToObject);

            float xDist = xPos - colObject.midPos[0];
            float yDist = yPos - colObject.midPos[1];

            vectorDistance[0] = xDist;
            vectorDistance[1] = yDist;

            normalizedDistance[0] = vectorDistance[0]/distanceToObject;
            normalizedDistance[1] = vectorDistance[1]/distanceToObject;

            xPos = colObject.midPos[0] + normalizedDistance[0] * colObject.colRadius;
            yPos = colObject.midPos[1] + normalizedDistance[1] * colObject.colRadius;

            renderPos.x = round(xPos);
            renderPos.y = round(yPos);
            onOrigin = false;
        }
    }
}

void backgroundPoint::render(SDL_Renderer *renderer)
{
    //get current logic render size from renderer
    int logicWidth, logicHeight;
    SDL_RenderGetLogicalSize(renderer, &logicWidth, &logicHeight);

    //get bigger background points by lowering the logic renderer size
    int newlogiclogicWidth =    round(logicWidth/pointSizeScale); 
    int newlogicHeight =        round(logicHeight/pointSizeScale);
    SDL_RenderSetLogicalSize(renderer, newlogiclogicWidth, newlogicHeight);

    //set the point render position according to the new logic render size
    int scaledPosX = round(renderPos.x/pointSizeScale); 
    int scaledPosY = round(renderPos.y/pointSizeScale); 

    //render points
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
    SDL_RenderDrawPoint(renderer,scaledPosX,scaledPosY);

    //reset logic render size
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
        std::vector<backgroundPoint> backgroundPointColumn;
        for (int j = 0; j < divider; j++)
        {
            int yPos = height/(float)divider * j + height/(float)divider/2.0f;
            backgroundPoint newPoint(xPos,yPos); 
            backgroundPointColumn.push_back(newPoint);
        }
        pointCloud.push_back(backgroundPointColumn);
    }  
}

//void createBackgroundPoints()
//{}

void background::update(std::list<GameObject>colObjects, float *updateTime)
{      
    int updatePointOperations = 0; 
    for (const GameObject &object : colObjects)
    {
        if (object.isVisible)
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
                            updatePointOperations++;                 
                        }
                    }
                }
            }
        }
    }

    for (std::vector<backgroundPoint> &backgroundPointColumn : pointCloud)
    {
        for (backgroundPoint &singleBgPoint : backgroundPointColumn)
            if (!singleBgPoint.onOrigin)
            {   
                singleBgPoint.returnToOrigin(updateTime);
                updatePointOperations++;
            }    
    }
    //std::cout << "Update Point Operations: " << updatePointOperations << std::endl;
}


void background::render(SDL_Renderer *renderer)
{
    for (std::vector<backgroundPoint> &backgroundPointColumn : pointCloud)
    {
        for (backgroundPoint &singleBackgroundPoint : backgroundPointColumn)
        {
            if (!singleBackgroundPoint.onOrigin)
            {   
                singleBackgroundPoint.render(renderer);
            }  
        }  
    }
}


