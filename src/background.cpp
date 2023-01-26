#include "background.hpp"

backgroundPoint::backgroundPoint()
{

}

backgroundPoint::backgroundPoint(float xPos, float yPos)
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
    squareDistanceToOrigin = (originPos.x - xPos) * (originPos.x - xPos) + (originPos.y - yPos) * (originPos.y - yPos);
    distanceToOrigin = SDL_sqrtf(squareDistanceToOrigin);

    vectorDistance[0] = (originPos.x - xPos);
    vectorDistance[1] = (originPos.y - yPos);
    normalizedDistance[0] = (vectorDistance[0] / (distanceToOrigin));
    normalizedDistance[1] = (vectorDistance[1] / (distanceToOrigin));

    float distanceDependentVelocity = distanceVelocityFunctionSteepness * distanceToOrigin * distanceToOrigin;

    float returnToOriginVelocity;
    returnToOriginVelocity = std::max(minReturnVelocity, distanceDependentVelocity) * *deltaTime * 60;

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
    int newlogiclogicWidth =    round(logicWidth / pointSizeScale); 
    int newlogicHeight =        round(logicHeight / pointSizeScale);
    SDL_RenderSetLogicalSize(renderer, newlogiclogicWidth, newlogicHeight);

    //set the point render position according to the new logic render size
    int scaledPosX = round(renderPos.x / pointSizeScale); 
    int scaledPosY = round(renderPos.y / pointSizeScale); 

    //render points
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
    SDL_RenderDrawPoint(renderer, scaledPosX, scaledPosY);

    //reset logic render size
    SDL_RenderSetLogicalSize(renderer, logicWidth, logicHeight);
}

background::background()
{

}

background::background(int windowWidth, int windowHeight)
{
    this->height = windowHeight;
    this->width = windowWidth;
    
    pointAreaWidth = width / static_cast<float>(divider);
    pointAreaHeight = height / static_cast<float>(divider);

    //create background points
    for (int i = 0; i != divider; i++)
    {     
        float newPointXPos = pointAreaWidth * i + pointAreaWidth / 2.0f;

        for (int j = 0; j < divider; j++)
        {
            float newPointYPos  = pointAreaHeight * j + pointAreaHeight / 2.0f;

            backgroundPoint newPoint = backgroundPoint(newPointXPos, newPointYPos); 
            backgroundPoints[i][j] = newPoint;
        }
    }  
}

void background::update(std::list<GameObject> colObjects, float *updateTime)
{    
    // debug, count update operations  
    int updatePointOperations = 0; 

    //look for collisions with objects
    for (const GameObject &object : colObjects)
    {
        if (object.isVisible)
        {              
            //locate point area of object
            int objectAreaPosX = object.midPos[0] / pointAreaWidth;
            int objectAreaPosY = object.midPos[1] / pointAreaHeight;

            //defince area size of collision detection box
            int collisionBoxWidth =     object.colRadius / pointAreaWidth;
            int collisionBoxHeight =    object.colRadius / pointAreaHeight;

            for (int i = objectAreaPosX - collisionBoxWidth; i <= objectAreaPosX + collisionBoxWidth; i++)
            {
                for (int j = objectAreaPosY - collisionBoxHeight; j <= objectAreaPosY + collisionBoxHeight; j++)
                {
                    //dont look for collsion outside of the visible area
                    if ((i >= 0 && i <= divider - 1) && (j >= 0 && j <= divider-1))
                    {
                        backgroundPoints[i][j].update(object); 
                        updatePointOperations++;                 
                    }
                }
            }
        }
    }

    for (int i = 0; i != divider; i++)
    {
        for (int j = 0; j != divider; j++)
            if (!backgroundPoints[i][j].onOrigin)
            {   
                backgroundPoints[i][j].returnToOrigin(updateTime);
                updatePointOperations++;
            }    
    }

    //debug 
    //std::cout << "Update Point Operations: " << updatePointOperations << std::endl;
}


void background::render(SDL_Renderer *renderer)
{
    //only render points not on origin
    for (int i = 0; i != divider; i++)
    {
        for (int j = 0; j != divider; j++)
            if (!backgroundPoints[i][j].onOrigin)
            {   
                backgroundPoints[i][j].render(renderer);
            }    
    }
}