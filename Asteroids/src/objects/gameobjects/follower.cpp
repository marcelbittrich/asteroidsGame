#include "follower.h"

#include "../../game.hpp"
#include "ship.h"
#include <vector>

Follower::Follower(Vec2 midPos, int size)
	: Enemy(midPos, 0.f)
{
	m_width = size;
	m_height = size;
	m_colRadius = size / 2.f;
	objectType = Type::Follower;
}

void Follower::Update(float deltaTime)
{
	UpdateTransform(deltaTime);
	UpdateAnimation(deltaTime);
}

void Follower::UpdateTransform(float deltaTime)
{
	float scalarVelocity = m_velocity.Length() * m_velocityDecay;
	m_velocity = m_velocity.Normalize() * scalarVelocity;

	// Get ship, move to ship
	Ship& ship = Game::ships.back();
	Vec2 shipPosition = ship.GetMidPos();
	Vec2 distance = shipPosition - m_midPos;

	Vec2 newVelocity = m_velocity;
	if (ship.GetVisibility())
	{
		newVelocity += distance.Normalize() * m_followSpeed;
	}
	else
	{
		newVelocity += distance.Normalize() * -m_backOfSpeed;
	}
 
	if (newVelocity.SquareLength() > m_velocityMax * m_velocityMax)
	{
		m_velocity = newVelocity.Normalize() * m_velocityMax;
	}
	else
	{
		m_velocity = newVelocity;
	}

	m_midPos += m_velocity * deltaTime;

	m_midPos = calcPosIfLeavingScreen(m_midPos, m_colRadius);
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
