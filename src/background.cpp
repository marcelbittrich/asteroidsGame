#include "background.hpp"

#include <vector>
#include <algorithm>
#include <execution>
#include <iostream>
#include "gameobjects.hpp"

BackgroundPoint::BackgroundPoint()
	: currentPos(0.f, 0.f), originPos(0.f, 0.f)
{
	color[0] = 255;
	color[1] = 0;
	color[2] = 255;
	color[3] = 255;

	onOrigin = false;
}

BackgroundPoint::BackgroundPoint(float xPos, float yPos)
	: currentPos(xPos, yPos), originPos(xPos, yPos)
{
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
				Vec2 currentPos = backgroundPoints[i][j].currentPos;
				Uint8* color = backgroundPoints[i][j].color;

				// Set the point position according to the new surface size
				Vec2 scaledPos = currentPos / m_pointSizeScale;

				Uint32 pixel = SDL_MapRGBA(backgroundSurface->format, color[0], color[1], color[2], color[3]);

				if (scaledPos.x < backgroundSurface->w && scaledPos.x > 0
					&& scaledPos.y < backgroundSurface->h && scaledPos.y > 0)
				{
					setPixel(backgroundSurface, (int)scaledPos.x, (int)scaledPos.y, pixel);
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

	float distance = Vec2::Distance(point.currentPos, point.originPos);
	Vec2 direction = point.originPos - point.currentPos;
	Vec2 normalizedVector = direction / distance;

	// points come back faster with more distance
	float distanceDependentVelocity = m_distanceVelocityFunctionSteepness * distance * distance;
	float returnToOriginVelocity = std::max(m_minReturnVelocity, distanceDependentVelocity) * deltaTime * 60.f;

	Vec2 changeVector = normalizedVector * returnToOriginVelocity;

	// if point would overshoot origin, put it on origin
	if (changeVector.SquareLength() > distance * distance)
	{
		point.currentPos = point.originPos;
		point.onOrigin = true;
	}
	else
	{
		point.currentPos += changeVector;
		point.onOrigin = false;
	}
}

void Background::movePointOut(BackgroundPoint& point, GameObject colObject)
{
	float objectColRadius = colObject.getColRadius();
	Vec2 objectMidPos = Vec2(colObject.getMidPos().x, colObject.getMidPos().y); //TODO: delete when GameObjects got refactored with Vec2

	// AABB collision check
	bool bOverlapHorizontally = point.currentPos.x > (objectMidPos.x - objectColRadius) 
		&& point.currentPos.x < (objectMidPos.x + objectColRadius);
	bool bOverlapVertically = point.currentPos.y > (objectMidPos.y - objectColRadius) 
		&& point.currentPos.y < (objectMidPos.y + objectColRadius);

	if (bOverlapHorizontally && bOverlapVertically)
	{
		// Distance based collision check
		bool bCollide = Vec2::SquareDistance(objectMidPos, point.currentPos) <= (objectColRadius * objectColRadius);
		// Push points out to the edge of the colliding object
		if (bCollide)
		{
			Vec2 direction = point.currentPos - objectMidPos;
			float distance = Vec2::Distance(objectMidPos, point.currentPos);

			Vec2 normalizedVector = direction / distance;

			// Push point to edge of colRadius
			point.currentPos = objectMidPos + normalizedVector * objectColRadius;
			point.onOrigin = false;
		}
	}
}