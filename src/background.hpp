#pragma once

#include "SDL.h"
#include <list>
#include <vector>

class BackgroundPoint
{
public:
	BackgroundPoint();
	BackgroundPoint(float xPos, float yPos);
	SDL_FPoint currentPos;
	SDL_Point originPos;
	SDL_Point renderPos;
	Uint8 color[4];
	bool onOrigin;
};

class Background
{
public:
	Background();
	Background(int windowWidth, int windowHeight);
	void Update(const std::list<class GameObject>& colObjects, float deltaTime);
	void Render(SDL_Renderer* renderer);

private:
	float m_pointSizeScale = 2.f;
	int m_width = 0;
	int m_height = 0;

	static const int divider = 100;
	BackgroundPoint backgroundPoints[divider][divider];

	std::vector<uint32_t> horizontalIter, verticalIter;
	void returnPointToOrigin(BackgroundPoint& point, float deltaTime);
	void movePointOut(BackgroundPoint& point, class GameObject colObject);

	float m_minReturnVelocity = 0.5f;
	float m_distanceVelocityFunctionSteepness = 0.005f;
	float m_putOnOriginDistance = 4.0f;

	void setPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
};
