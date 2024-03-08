#include "background.hpp"

#include <vector>
#include <algorithm>
#include <execution>
#include <iostream>

#include "../objects/gameobjects/gameobject.hpp"

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

Background::Background(int windowWidth, int windowHeight, float pointScale, SDL_Renderer* renderer)
	: m_pointSizeScale(pointScale), m_renderer(renderer)
{
	// Devide the screen in areas, each occupied by one point.
	float pointAreaWidth = windowWidth / (float)(divider);
	float pointAreaHeight = windowHeight / (float)(divider);

	// Iterators of for_each loops, for parallel execution.
	for (uint32_t i = 0; i < divider; i++)
	{
		horizontalIter.push_back(i);
		verticalIter.push_back(i);
	}

	backgroundPoints = new BackgroundPoint[divider * divider];

	// Create background points in the middle of each area.
	for (int i = 0; i < divider; i++)
	{
		float newPointXPos = pointAreaWidth * i + pointAreaWidth / 2.0f;

		for (int j = 0; j < divider; j++)
		{
			float newPointYPos = pointAreaHeight * j + pointAreaHeight / 2.0f;
			backgroundPoints[i * divider + j] = BackgroundPoint(newPointXPos, newPointYPos);
		}
	}

	// Get bigger background points by lowering the surface size
	SDL_RenderGetLogicalSize(m_renderer, &m_logicWidth, &m_logicHeight);
	m_surfaceWidth = (int)round(m_logicWidth / m_pointSizeScale);
	m_surfaceHeight = (int)round(m_logicHeight / m_pointSizeScale);
	m_backgroundSurface = SDL_CreateRGBSurface(0, m_surfaceWidth, m_surfaceHeight, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
}

void Background::Update(const std::vector<GameObject*>& gameObjects, float deltaTime)
{
	m_gameObjects = &gameObjects;
	m_deltaTime = deltaTime;

	std::for_each(std::execution::par, verticalIter.begin(), verticalIter.end(),
		[this](uint32_t i)
		{
			std::for_each(horizontalIter.begin(), horizontalIter.end(),
			[this, i](uint32_t j)
				{
					BackgroundPoint& point = backgroundPoints[i * divider + j];
					MovePointOut(point);
					if (!point.onOrigin)
					{
						ReturnPointToOrigin(point);
					}
				});
		});
}

void Background::Render()
{
	//Set background to black
	SDL_FillRect(m_backgroundSurface, NULL, 0x000000ff);

	// Only render points not on origin
	for (int i = 0; i != divider; i++)
	{
		for (int j = 0; j != divider; j++)
			if (!backgroundPoints[i * divider + j].onOrigin)
			{
				Vec2 currentPos = backgroundPoints[i * divider + j].currentPos;
				Uint8* color = backgroundPoints[i * divider + j].color;

				// Set the point position according to the new surface size
				Vec2 scaledPos = currentPos / m_pointSizeScale;

				Uint32 pixel = SDL_MapRGBA(m_backgroundSurface->format, color[0], color[1], color[2], color[3]);

				if (scaledPos.x < m_backgroundSurface->w && scaledPos.x > 0
					&& scaledPos.y < m_backgroundSurface->h && scaledPos.y > 0)
				{
					SetPixel(m_backgroundSurface, (int)scaledPos.x, (int)scaledPos.y, pixel);
				}
			}
	}

	// Set sampling to nearest pixel
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(m_renderer, m_backgroundSurface);
	SDL_Rect textureRect = { 0, 0, m_surfaceWidth, m_surfaceHeight };
	SDL_Rect backgroundRect = { 0, 0, m_logicWidth, m_logicHeight };

	// Destroy texture and reset sampling method
	SDL_RenderCopy(m_renderer, backgroundTexture, &textureRect, &backgroundRect);
	SDL_DestroyTexture(backgroundTexture);
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
}

void Background::SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	Uint32* pixels = (Uint32*)surface->pixels;
	int position = (y * surface->w) + x;
	pixels[position] = pixel;
}

void Background::ReturnPointToOrigin(BackgroundPoint& point) const
{
	float distance = Vec2::Distance(point.currentPos, point.originPos);
	Vec2 direction = point.originPos - point.currentPos;
	Vec2 normalizedVector = direction / distance;

	// Points come back faster with more distance
	float distanceDependentVelocity = m_distanceVelocityFunctionSteepness * distance * distance;
	float returnToOriginVelocity = std::max(m_minReturnVelocity, distanceDependentVelocity) * m_deltaTime;
	
	Vec2 changeVector = normalizedVector * returnToOriginVelocity;

	// If point would overshoot origin or is very far from origin, put it on origin
	if ((changeVector.SquareLength() > distance * distance) || (distance > m_maxPointDistance))
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

void Background::MovePointOut(BackgroundPoint& point)
{
	for (const GameObject* colObject : *m_gameObjects) 
	{
		if (!colObject->GetVisibility())
			continue;

		float objectColRadius = colObject->GetColRadius();
		Vec2 objectMidPos = colObject->GetMidPos();

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
}