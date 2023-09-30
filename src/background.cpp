#include "background.hpp"

#include <vector>
#include <algorithm>
#include <execution>
#include <iostream>
#include "gameobjects.hpp"

namespace Utils
{
    float squareDistance(SDL_FPoint PositionA, SDL_FPoint PositionB)
    {
        float squareDistanceX = (PositionA.x - PositionB.x) * (PositionA.x - PositionB.x);
        float squareDistanceY = (PositionA.y - PositionB.y) * (PositionA.y - PositionB.y);
        return squareDistanceX + squareDistanceY;
    }
}

BackgroundPoint::BackgroundPoint()
{
}

BackgroundPoint::BackgroundPoint(float xPos, float yPos)
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

Background::Background()
{
}

Background::Background(int windowWidth, int windowHeight)
{
    height = windowHeight;
    width = windowWidth;

    // Devide the screen in areas each occupied by one point
    pointAreaWidth = width / (float)(divider);
    pointAreaHeight = height / (float)(divider);

    horizontalIter.resize(divider);
    verticalIter.resize(divider);

    for (uint32_t i = 0; i < divider; i++)
    {
        horizontalIter[i] = i;
        verticalIter[i] = i;
    }

    // create background points in the middle of each area
    for (int i = 0; i < divider; i++)
    {
        float newPointXPos = pointAreaWidth * i + pointAreaWidth / 2.0f;

        for (int j = 0; j < divider; j++)
        {
            float newPointYPos = pointAreaHeight * j + pointAreaHeight / 2.0f;

            BackgroundPoint newPoint = BackgroundPoint(newPointXPos, newPointYPos);
            backgroundPoints[i][j] = newPoint;
        }
    }

    std::cout << "Size of Background point: " << sizeof(backgroundPoints[1][1]) << std::endl;
    std::cout << "Size of whole Background: " << sizeof(*this) << std::endl;
}

void Background::update(const std::list<GameObject> &colObjects, float deltaTime)
{

#define NO_MT 1
#if MT
    // debug, count update operations
    int updatePointOperations = 0;

    // look for collisions with objects
    for (const GameObject &object : colObjects)
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
                        movePointOut(backgroundPoints[i][j], object);
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
                returnPointToOrigin(backgroundPoints[i][j], deltaTime);
            }
    }

#else
    for (const GameObject &object : colObjects)
    {
        if (object.getVisibility())
        {
            GameObject colObject = object;
            std::for_each(std::execution::par, verticalIter.begin(), verticalIter.end(),
                          [this, colObject](uint32_t i)
                          {
                              std::for_each(horizontalIter.begin(), horizontalIter.end(),
                                            [this, i, colObject](uint32_t j)
                                            {
                                                movePointOut(backgroundPoints[i][j], colObject);
                                            });
                          });
        }
    }

    std::for_each(std::execution::par, verticalIter.begin(), verticalIter.end(),
                  [this, deltaTime](uint32_t i)
                  {
                      std::for_each(horizontalIter.begin(), horizontalIter.end(),
                                    [this, i, deltaTime](uint32_t j)
                                    {
                                        if (!backgroundPoints[i][j].onOrigin)
                                        {
                                            returnPointToOrigin(backgroundPoints[i][j], deltaTime);
                                        }
                                    });
                  });

    std::cout << "Size of whole colobjects: " << sizeof(colObjects) << std::endl;

#endif
    // debug
    // std::cout << "Update Point Operations: " << updatePointOperations << std::endl;
}

void Background::render(SDL_Renderer *renderer)
{
    // only render points not on origin
    for (int i = 0; i != divider; i++)
    {
        for (int j = 0; j != divider; j++)
            if (!backgroundPoints[i][j].onOrigin)
            {
                SDL_Point renderPos = backgroundPoints[i][j].renderPos;
                Uint8 *color = backgroundPoints[i][j].color;

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
    }
}

void Background::returnPointToOrigin(BackgroundPoint &point, float deltaTime)
{

    SDL_FPoint originPosF = {(float)point.originPos.x, (float)point.originPos.y};
    float squareDistanceToOrigin = Utils::squareDistance(point.currentPos, originPosF);
    float distanceToOrigin = SDL_sqrtf(squareDistanceToOrigin);

    float vectorDistance[2];
    vectorDistance[0] = (point.originPos.x - point.currentPos.x);
    vectorDistance[1] = (point.originPos.y - point.currentPos.y);

    float normalizedDistance[2];
    normalizedDistance[0] = (vectorDistance[0] / distanceToOrigin);
    normalizedDistance[1] = (vectorDistance[1] / distanceToOrigin);

    // points come back faster with more distance
    float distanceDependentVelocity = distanceVelocityFunctionSteepness * distanceToOrigin * distanceToOrigin;
    float returnToOriginVelocity = std::max(minReturnVelocity, distanceDependentVelocity) * deltaTime * 60.f;

    float vectorChange[2];
    vectorChange[0] = (normalizedDistance[0] * returnToOriginVelocity);
    vectorChange[1] = (normalizedDistance[1] * returnToOriginVelocity);

    // if point would overshoot origin, put it on origin
    if (vectorChange[0] * vectorChange[0] + vectorChange[1] * vectorChange[1] > squareDistanceToOrigin)
    {
        point.currentPos.x = point.originPos.x;
        point.currentPos.y = point.originPos.y;

        point.renderPos.x = point.originPos.x;
        point.renderPos.y = point.originPos.y;

        point.onOrigin = true;
    }
    else
    {
        point.currentPos.x += vectorChange[0];
        point.currentPos.y += vectorChange[1];

        point.renderPos.x = round(point.currentPos.x);
        point.renderPos.y = round(point.currentPos.y);

        point.onOrigin = false;
    }
}

void Background::movePointOut(BackgroundPoint &point, GameObject colObject)
{
    float objectColRadius = colObject.getColRadius();
    SDL_FPoint objectMidPos = colObject.getMidPos();

    // AABB collision check
    bool bOverlapHorizontally = point.currentPos.x > (objectMidPos.x - objectColRadius) && point.currentPos.x < (objectMidPos.x + objectColRadius);
    bool bOverlapVertically = point.currentPos.y > (objectMidPos.y - objectColRadius) && point.currentPos.y < (objectMidPos.y + objectColRadius);

    if (bOverlapHorizontally && bOverlapVertically)
    {
        // distance based collision check
        bool bCollide = Utils::squareDistance(objectMidPos, point.currentPos) <= (objectColRadius * objectColRadius);

        // push out points to the edge of the colliding object
        if (bCollide)
        {
            //
            float VectorMidToPoint[2];
            VectorMidToPoint[0] = point.currentPos.x - objectMidPos.x;
            VectorMidToPoint[1] = point.currentPos.y - objectMidPos.y;

            float distanceToObject = SDL_sqrtf(Utils::squareDistance(objectMidPos, point.currentPos));
            float VectorNormalized[2];
            VectorNormalized[0] = VectorMidToPoint[0] / distanceToObject;
            VectorNormalized[1] = VectorMidToPoint[1] / distanceToObject;

            // push point to edge of colRadius
            point.currentPos.x = objectMidPos.x + VectorNormalized[0] * objectColRadius;
            point.currentPos.y = objectMidPos.y + VectorNormalized[1] * objectColRadius;

            point.renderPos.x = round(point.currentPos.x);
            point.renderPos.y = round(point.currentPos.y);

            point.onOrigin = false;
        }
    }
}