#include "powerup.h"

#include "SDL_rect.h"
#include "SDL_render.h"
#include "ship.h"
#include "../../audio/audioplayer.hpp"

PowerUp::PowerUp(Vec2 midPos, Vec2 velocity)
	: Collectable(midPos, velocity)
{
	m_width = m_size;
	m_height = m_size;

	objectType = Type::PowerUp;

	m_colRadius = m_size * m_colRadiusFactor;
}

void PowerUp::Update(float deltaTime)
{
	m_midPos += m_velocity * deltaTime;
	m_midPos = calcPosIfLeavingScreen(m_midPos, m_colRadius);
	m_rotation += m_rotatingSpeed * deltaTime;
}

void PowerUp::Render()
{
	SDL_FRect rect = GetRenderRect();
	SDL_RenderCopyExF(s_renderer, s_texture, NULL, &rect, m_rotation, NULL, SDL_FLIP_NONE);
}

void PowerUp::GetCollected(Ship* ownerShip)
{
	s_audioPlayer->PlaySoundEffect(EffectType::CollectedSound);
	ownerShip->CollectPowerUp();
	m_isDead = true;
}
