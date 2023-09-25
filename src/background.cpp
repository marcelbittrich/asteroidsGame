#include "background.hpp"

#include <vector>
#include "gameobjects.hpp"

#pragma region BackgroundPoint

namespace Utils
{
    float squareDistance(SDL_FPoint PositionA, SDL_FPoint PositionB)
    {
        float squareDistanceX = (PositionA.x - PositionB.x) * (PositionA.x - PositionB.x);
        float squareDistanceY = (PositionA.y - PositionB.y) * (PositionA.y - PositionB.y);
        return squareDistanceX + squareDistanceY;
    }
}

backgroundPoint::backgroundPoint()
{
}

backgroundPoint::backgroundPoint(float xPos, float yPos)
{
    currentPos.x = xPos;
    currentPos.y = yPos;

    originPos.x = xPos;
    originPos.y = yPos;

    renderPos.x = xPos;
    renderPos.y = yPos;

    color[0] = rand() % 255;
    color[1] = rand() % 255;
    color[2] = rand() % 255;
    color[3] = 255;

    onOrigin = true;
}

void backgroundPoint::returnToOrigin(float deltaTime)
{
    SDL_FPoint originPosF = {originPos.x, originPos.y};
    squareDistanceToOrigin = Utils::squareDistance(currentPos, originPosF);
    distanceToOrigin = SDL_sqrtf(squareDistanceToOrigin);

    float vectorDistance[2];
    vectorDistance[0] = (originPos.x - currentPos.x);
    vectorDistance[1] = (originPos.y - currentPos.y);

    float normalizedDistance[2];
    normalizedDistance[0] = (vectorDistance[0] / distanceToOrigin);
    normalizedDistance[1] = (vectorDistance[1] / distanceToOrigin);

    // points come back faster with more distance
    float distanceDependentVelocity = distanceVelocityFunctionSteepness * distanceToOrigin * distanceToOrigin;
    float returnToOriginVelocity = std::max(minReturnVelocity, distanceDependentVelocity) * deltaTime * 60.f;

    vectorChange[0] = (normalizedDistance[0] * returnToOriginVelocity);
    vectorChange[1] = (normalizedDistance[1] * returnToOriginVelocity);

    // if point would overshoot origin, put it on origin
    if (vectorChange[0] * vectorChange[0] + vectorChange[1] * vectorChange[1] > squareDistanceToOrigin)
    {
        currentPos.x = originPos.x;
        currentPos.y = originPos.y;

        renderPos.x = originPos.x;
        renderPos.y = originPos.y;

        onOrigin = true;
    }
    else
    {
        currentPos.x += vectorChange[0];
        currentPos.y += vectorChange[1];

        renderPos.x = round(currentPos.x);
        renderPos.y = round(currentPos.y);

        onOrigin = false;
    }
}

void backgroundPoint::moveOut(GameObject colObject)
{
    float objectColRadius = colObject.getColRadius();
    SDL_FPoint objectMidPos = colObject.getMidPos();

    // AABB collision check
    bool bOverlapHorizontally = currentPos.x > (objectMidPos.x - objectColRadius) && currentPos.x < (objectMidPos.x + objectColRadius);
    bool bOverlapVertically = currentPos.y > (objectMidPos.y - objectColRadius) && currentPos.y < (objectMidPos.y + objectColRadius);

    if (bOverlapHorizontally && bOverlapVertically)
    {
        // distance based collision check
        bool bCollide = Utils::squareDistance(objectMidPos, currentPos) <= (objectColRadius * objectColRadius);

        // push out points to the edge of the colliding object
        if (bCollide)
        {
            //
            float VectorMidToPoint[2];
            VectorMidToPoint[0] = currentPos.x - objectMidPos.x;
            VectorMidToPoint[1] = currentPos.y - objectMidPos.y;

            float distanceToObject = SDL_sqrtf(Utils::squareDistance(objectMidPos, currentPos));
            float VectorNormalized[2];
            VectorNormalized[0] = VectorMidToPoint[0] / distanceToObject;
            VectorNormalized[1] = VectorMidToPoint[1] / distanceToObject;

            // push point to edge of colRadius
            currentPos.x = objectMidPos.x + VectorNormalized[0] * objectColRadius;
            currentPos.y = objectMidPos.y + VectorNormalized[1] * objectColRadius;

            renderPos.x = round(currentPos.x);
            renderPos.y = round(currentPos.y);

            onOrigin = false;
        }
    }
}

void backgroundPoint::render(SDL_Renderer *renderer)
{
    // get current logic render size from renderer
    int logicWidth, logicHeight;
    SDL_RenderGetLogicalSize(renderer, &logicWidth, &logicHeight);

    // get bigger background points by lowering the logic renderer size
    int newlogiclogicWidth = round(logicWidth / pointSizeScale);
    int newlogicHeight = round(logicHeight / pointSizeScale);
    SDL_RenderSetLogicalSize(renderer, newlogiclogicWidth, newlogicHeight);

    // set the point render position according to the new logic render size
    int scaledPosX = round(renderPos.x / pointSizeScale);
    int scaledPosY = round(renderPos.y / pointSizeScale);

    // render points
    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
    SDL_RenderDrawPoint(renderer, scaledPosX, scaledPosY);

    // reset logic render size
    SDL_RenderSetLogicalSize(renderer, logicWidth, logicHeight);
}

#pragma endregion

#pragma region Background

background::background()
{
}

background::background(int windowWidth, int windowHeight)
{
    height = windowHeight;
    width = windowWidth;

    // Devide the screen in areas each occupied by one point
    pointAreaWidth = width / static_cast<float>(divider);
    pointAreaHeight = height / static_cast<float>(divider);

    // create background points in the middle of each area
    for (int i = 0; i != divider; i++)
    {
        float newPointXPos = pointAreaWidth * i + pointAreaWidth / 2.0f;

        for (int j = 0; j < divider; j++)
        {
            float newPointYPos = pointAreaHeight * j + pointAreaHeight / 2.0f;

            backgroundPoint newPoint = backgroundPoint(newPointXPos, newPointYPos);
            backgroundPoints[i][j] = newPoint;
        }
    }
}

void background::update(std::list<GameObject> colObjects, float deltaTime)
{
    // debug, count update operations
    int updatePointOperations = 0;

    // look for collisions with objects
    for (GameObject &object : colObjects)
    {
        float objectColRadius = object.getColRadius();
        SDL_FPoint objectMidPos = object.getMidPos();

        if (object.getVisibility())
        {
            // locate point area of object
            int objectAreaPosX = objectMidPos.x / pointAreaWidth;
            int objectAreaPosY = objectMidPos.y / pointAreaHeight;

            // defince area size of collision detection box
            int collisionBoxWidth = objectColRadius / pointAreaWidth;
            int collisionBoxHeight = objectColRadius / pointAreaHeight;

            for (int i = objectAreaPosX - collisionBoxWidth; i <= objectAreaPosX + collisionBoxWidth; i++)
            {
                for (int j = objectAreaPosY - collisionBoxHeight; j <= objectAreaPosY + collisionBoxHeight; j++)
                {
                    // dont look for collsion outside of the visible area
                    if ((i >= 0 && i <= divider - 1) && (j >= 0 && j <= divider - 1))
                    {
                        /* Edge Distance Filter / Optimizazion
                        float midPosDistance = SDL_sqrtf(Utils::squareDistance(object.midPos, backgroundPoints[i][j].currentPos));
                        float distanceToColEdge = object.colRadius - midPosDistance;
                        float edgeDistanceThreshold = 100.f;

                        if (distanceToColEdge < edgeDistanceThreshold)
                        {

                        }
                        */

                        backgroundPoints[i][j].moveOut(object);
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
                backgroundPoints[i][j].returnToOrigin(deltaTime);
            }
    }

    // debug
    std::cout << "Update Point Operations: " << updatePointOperations << std::endl;
}

void background::render(SDL_Renderer *renderer)
{
    // only render points not on origin
    for (int i = 0; i != divider; i++)
    {
        for (int j = 0; j != divider; j++)
            if (!backgroundPoints[i][j].onOrigin)
            {
                backgroundPoints[i][j].render(renderer);
            }
    }
}

#pragma endregion