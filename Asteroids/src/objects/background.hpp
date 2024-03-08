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
	Background(int windowWidth, int windowHeight, float pointScale, SDL_Renderer* renderer);
	void Update(const std::vector<class GameObject*>& gameObjects, float deltaTime);
	void Render();

private:
	static const int divider = 100;
	BackgroundPoint* backgroundPoints = nullptr;
	float m_pointSizeScale = 2.f;

	std::vector<uint32_t> horizontalIter, verticalIter;
	const std::vector<class GameObject*>* m_gameObjects = nullptr;
	float m_deltaTime = 0.f;
	void ReturnPointToOrigin(BackgroundPoint& point) const;
	void MovePointOut(BackgroundPoint& point);

	SDL_Renderer* m_renderer = nullptr;
	SDL_Surface* m_backgroundSurface = nullptr;
	int m_logicWidth = 0;
	int m_logicHeight = 0;
	int m_surfaceWidth = 0;
	int m_surfaceHeight = 0;

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
