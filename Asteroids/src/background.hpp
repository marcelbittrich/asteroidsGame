#pragma once

#include "SDL.h"
#include <list>
#include <vector>

#include "vector2.hpp"


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
	Background();
	Background(int windowWidth, int windowHeight, float pointScale);
	void Update(const std::list<class GameObject>& gameObjects, float deltaTime);
	void Render(SDL_Renderer* renderer);

private:
	static const int divider = 100;
	BackgroundPoint backgroundPoints[divider][divider];
	float m_pointSizeScale = 2.f;

	std::vector<uint32_t> horizontalIter, verticalIter;
	void returnPointToOrigin(BackgroundPoint& point, float deltaTime);
	void movePointOut(BackgroundPoint& point, class GameObject colObject);

	float m_minReturnVelocity = 0.5f;
	float m_distanceVelocityFunctionSteepness = 0.005f;
	float m_putOnOriginDistance = 4.0f;

	void setPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
};
