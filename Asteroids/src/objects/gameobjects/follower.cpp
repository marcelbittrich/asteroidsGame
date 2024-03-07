#include "follower.h"

#include "../../game.hpp"
#include <vector>

Follower::Follower(Vec2 midPos, int size)
	: GameObject(midPos, 0.f)
{
	m_width = size;
	m_height = size;

	objectType = Type::Follower;
}

void Follower::Update(float deltaTime)
{
	UpdateTransform(deltaTime);
	UpdateAnimation(deltaTime);
}

void Follower::UpdateTransform(float deltaTime)
{
	// Get ship, move to ship
	Vec2 shipPosition = Game::ships.back().GetMidPos();

	Vec2 distance = shipPosition - m_midPos;
	m_velocity = distance.Normalize() * 50.f;

	m_midPos += m_velocity * deltaTime;
	m_midPos = calcPosIfLeavingScreen(m_midPos, 0);
}

void Follower::UpdateAnimation(float deltaTime)
{
	if (SDL_GetTicks() - m_timeLastUpdated > m_timeBetweenSprites)
	{
		m_animationCounter++;
		m_animationCounter = m_animationCounter % m_spriteCount + 1;
		m_timeLastUpdated = SDL_GetTicks();
	}
}

void Follower::Render()
{
	int currentSpriteStart = m_spriteWidth * m_animationCounter;
	SDL_Rect srcR{ currentSpriteStart,
				  0,
				  m_spriteWidth,
				  m_spriteHeight };
	SDL_FRect destR = GetRenderRect();

	SDL_RenderCopyExF(s_renderer, s_texture, &srcR, &destR, 0.0, NULL, SDL_FLIP_NONE);
}
