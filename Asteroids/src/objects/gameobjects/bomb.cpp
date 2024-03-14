#include "bomb.h"

#include "SDL_rect.h"
#include "SDL_render.h"

#include "ship.h"
#include "../../audio/audioplayer.hpp"

Bomb::Bomb(Vec2 midPos, Vec2 velocity)
	: Collectable(midPos, velocity)
{
	m_isVisible = true;

	m_width = m_size;
	m_height = m_size;

	isCollected = false;
	isExploding = false;

	m_colRadius = m_size * m_colRadiusFactor;

	objectType = Type::Bomb;
}

void Bomb::Update(float deltaTime)
{
	if (!isCollected && !isExploding)
	{
		m_midPos += m_velocity * deltaTime;
		m_midPos = calcPosIfLeavingScreen(m_midPos, m_colRadius);
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
	s_audioPlayer->PlaySoundEffect(EffectType::CollectedSound);
	m_ownerShip = ownerShip;
	ownerShip->CollectBomb(this);
	isCollected = true;
	m_isVisible = false;
}

void Bomb::Explode()
{
	colType = CollisionType::Weapon;
	s_audioPlayer->PlaySoundEffect(EffectType::BombExplode);
	isExploding = true;
	m_isVisible = true;
	m_ownerShip = nullptr;
}