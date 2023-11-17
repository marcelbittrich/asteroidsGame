#include "gameobject.hpp"

#include "SDL_rect.h"

Vec2 calcPosIfLeaving(Vec2 m_midPos, float radius, int windowWidth, int windowHeight)
{
	Vec2 newMidPos = m_midPos;

	if (m_midPos.x < 0 - radius) // leave to left.
	{
		newMidPos.x = windowWidth + radius;
	}
	else if (m_midPos.x > windowWidth + radius) // leave to right.
	{
		newMidPos.x = 0 - radius;
	}

	if (m_midPos.y < 0 - radius) // leave to top.
	{
		newMidPos.y = windowHeight + radius;
	}
	else if (m_midPos.y > windowHeight + radius) // leave to bottom.
	{
		newMidPos.y = 0 - radius;
	}

	return newMidPos;
}

SDL_FRect GameObject::GetRenderRect() const
{
	SDL_FRect rect;
	rect.w = (float)m_width;
	rect.h = (float)m_height;
	rect.x = m_midPos.x - (float)m_width / 2.f;
	rect.y = m_midPos.y - (float)m_height / 2.f;
	return rect;
}
