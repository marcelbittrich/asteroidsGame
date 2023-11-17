#include "bomb.h"

#include "SDL_rect.h"
#include "SDL_render.h"

#include "ship.h"
#include "../../audio/audioplayer.hpp"

Bomb::Bomb(Vec2 midPos, Vec2 velocity)
	: GameObject(midPos, velocity)
{
	m_isVisible = true;

	m_width = m_size;
	m_height = m_size;

	isCollected = false;
	isExploding = false;

	m_colRadius = m_size * m_colRadiusFactor;

	objectType = Type::Bomb;

	bombs.push_back(*this);
}

void Bomb::Update(int windowWidth, int windowHeight, float deltaTime)
{
	if (!isCollected && !isExploding)
	{
		m_midPos.x += m_velocity.x * deltaTime * 60;
		m_midPos.y += m_velocity.y * deltaTime * 60;
		m_midPos = calcPosIfLeaving(m_midPos, m_colRadius, windowWidth, windowHeight);

		m_rotation += m_rotatingSpeed * deltaTime;
	}
	if (isCollected && !isExploding && m_ownerShip)
	{
		m_midPos = m_ownerShip->GetMidPos();
	}
	if (isExploding)
	{
		m_colRadius += m_explodindVelocity * deltaTime;
		if (m_colRadius > m_maxExplodingRadius)
		{
			m_colRadius = m_maxExplodingRadius;
			m_isDead = true;
		}
	}
}

void Bomb::Render()
{
	if (m_isVisible)
	{
		SDL_FRect rect = GetRenderRect();
		SDL_RenderCopyExF(s_renderer, s_texture, NULL, &rect, m_rotation, NULL, SDL_FLIP_NONE);
	}
}

void Bomb::GetCollected(Ship* ownerShip)
{
	isCollected = true;
	m_isVisible = false;
	m_ownerShip = ownerShip;
}

void Bomb::Explode()
{
	s_audioPlayer->PlaySoundEffect(EffectType::BombExplode);
	isExploding = true;
	m_isVisible = true;
	m_ownerShip = nullptr;
}