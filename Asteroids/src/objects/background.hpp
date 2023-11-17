#pragma once

#include <vector>

#include "SDL.h"

#include "../vector2.hpp"


struct BackgroundPoint
{
	BackgroundPoint();
	BackgroundPoint(float xPos, float yPos);
	Vec2 currentPos;
	Vec2 originPos;
	Uint8 color[4];
	bool onOrigin;
};

class Background
{
public:
	Background() {};
	Background(int windowWidth, int windowHeight, float pointScale);
	void Update(const std::vector<class GameObject*>& gameObjects, float deltaTime);
	void Render(SDL_Renderer* renderer);

private:
	static const int divider = 100;
	BackgroundPoint backgroundPoints[divider * divider];
	float m_pointSizeScale = 2.f;

	std::vector<uint32_t> horizontalIter, verticalIter;
	void ReturnPointToOrigin(BackgroundPoint& point, float deltaTime);
	void MovePointOut(BackgroundPoint& point, const class GameObject* colObject);

	// How fast the trailing points will return to origin.
	// Default: 30
	float m_minReturnVelocity = 30.f;
	// How fast the points right behind the object will retrun. 
	// Default: 0.3
	float m_distanceVelocityFunctionSteepness = 0.3f;
	// Mainly used for adjusting the look of the bomb explosion.
	// Default: 380
	float m_maxPointDistance = 380.0f; 

	void SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
};
