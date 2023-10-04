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
	currentPos.x = 0.f;
	currentPos.y = 0.f;

	originPos.x = 0;
	originPos.y = 0;

	renderPos.x = 0;
	renderPos.y = 0;

	color[0] = 255;
	color[1] = 255;
	color[2] = 255;
	color[3] = 255;

	onOrigin = false;
}

BackgroundPoint::BackgroundPoint(float xPos, float yPos)
{
	currentPos.x = xPos;
	currentPos.y = yPos;

	originPos.x = (int)xPos;
	originPos.y = (int)yPos;

	renderPos.x = (int)xPos;
	renderPos.y = (int)yPos;

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
	m_height = windowHeight;
	m_width = windowWidth;

	// Devide the screen in areas each occupied by one point
	float pointAreaWidth = m_width / (float)(divider);
	float pointAreaHeight = m_height / (float)(divider);

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

	//std::cout << "Size of Background point: " << sizeof(backgroundPoints[1][1]) << std::endl;
	//std::cout << "Size of whole Background: " << sizeof(*this) << std::endl;
}

void Background::Update(const std::list<GameObject>& gameObjects, float deltaTime)
{

#define NO_MT 0
#if NO_MT
	// debug, count update operations
	int updatePointOperations = 0;

	// look for collisions with objects
	for (const GameObject& object : colObjects)
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
	for (const GameObject& object : gameObjects)
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

#endif

}

void Background::Render(SDL_Renderer* renderer)
{
	// Get current logic render size from renderer
	int logicWidth, logicHeight;
	SDL_RenderGetLogicalSize(renderer, &logicWidth, &logicHeight);

	// Get bigger background points by lowering the surface size
	int surfaceWidth = (int)round(logicWidth / m_pointSizeScale);
	int surfaceHeight = (int)round(logicHeight / m_pointSizeScale);

	SDL_Surface* backgroundSurface = SDL_CreateRGBSurface(0, surfaceWidth, surfaceHeight, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	SDL_memset(backgroundSurface->pixels, 0, backgroundSurface->h * backgroundSurface->pitch);

	// Only render points not on origin
	for (int i = 0; i != divider; i++)
	{
		for (int j = 0; j != divider; j++)
			if (!backgroundPoints[i][j].onOrigin)
			{
				SDL_Point renderPos = backgroundPoints[i][j].renderPos;
				Uint8* color = backgroundPoints[i][j].color;

				// Set the point position according to the new surface size
				int scaledPosX = (int)round(renderPos.x / m_pointSizeScale);
				int scaledPosY = (int)round(renderPos.y / m_pointSizeScale);

				Uint32 pixel = SDL_MapRGBA(backgroundSurface->format, color[0], color[1], color[2], color[3]);

				if (scaledPosX < backgroundSurface->w && scaledPosX > 0
					&& scaledPosY < backgroundSurface->h && scaledPosY > 0)
				{
					setPixel(backgroundSurface, scaledPosX, scaledPosY, pixel);
				}
			}
	}

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
	SDL_FreeSurface(backgroundSurface);

	// Render the texture full screen
	SDL_Rect textureRect = { 0, 0, surfaceWidth, surfaceHeight };
	SDL_Rect backgroundRect = { 0, 0, logicWidth, logicHeight };

	// Destroy and Reset
	SDL_RenderCopy(renderer, backgroundTexture, &textureRect, &backgroundRect);
	SDL_DestroyTexture(backgroundTexture);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
}

void Background::setPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	Uint32* pixels = (Uint32*)surface->pixels;

	int position = (y * surface->w) + x;

	pixels[position] = pixel;
}

void Background::returnPointToOrigin(BackgroundPoint& point, float deltaTime)
{

	SDL_FPoint originPosF = { (float)point.originPos.x, (float)point.originPos.y };
	float squareDistanceToOrigin = Utils::squareDistance(point.currentPos, originPosF);
	float distanceToOrigin = SDL_sqrtf(squareDistanceToOrigin);

	float vectorDistance[2];
	vectorDistance[0] = (point.originPos.x - point.currentPos.x);
	vectorDistance[1] = (point.originPos.y - point.currentPos.y);

	float normalizedDistance[2];
	normalizedDistance[0] = (vectorDistance[0] / distanceToOrigin);
	normalizedDistance[1] = (vectorDistance[1] / distanceToOrigin);

	// points come back faster with more distance
	float distanceDependentVelocity = m_distanceVelocityFunctionSteepness * distanceToOrigin * distanceToOrigin;
	float returnToOriginVelocity = std::max(m_minReturnVelocity, distanceDependentVelocity) * deltaTime * 60.f;

	float vectorChange[2];
	vectorChange[0] = (normalizedDistance[0] * returnToOriginVelocity);
	vectorChange[1] = (normalizedDistance[1] * returnToOriginVelocity);

	// if point would overshoot origin, put it on origin
	if (vectorChange[0] * vectorChange[0] + vectorChange[1] * vectorChange[1] > squareDistanceToOrigin)
	{
		point.currentPos.x = (float)point.originPos.x;
		point.currentPos.y = (float)point.originPos.y;

		point.renderPos.x = point.originPos.x;
		point.renderPos.y = point.originPos.y;

		point.onOrigin = true;
	}
	else
	{
		point.currentPos.x += vectorChange[0];
		point.currentPos.y += vectorChange[1];

		point.renderPos.x = (int)round(point.currentPos.x);
		point.renderPos.y = (int)round(point.currentPos.y);

		point.onOrigin = false;
	}
}

void Background::movePointOut(BackgroundPoint& point, GameObject colObject)
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

			point.renderPos.x = (int)round(point.currentPos.x);
			point.renderPos.y = (int)round(point.currentPos.y);

			point.onOrigin = false;
		}
	}
}