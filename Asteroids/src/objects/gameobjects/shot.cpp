#include "shot.h"

#include "SDL_rect.h"
#include "SDL_render.h"

Shot::Shot(Vec2 midPos, Vec2 velocity, float rotation)
	: GameObject(midPos, velocity)
{
	m_rotation = rotation;
	m_colRadius = m_size * m_colRadiusFactor;

	m_width = m_size;
	m_height = m_size;

	objectType = Type::Shot;
}

void Shot::Update(float deltaTime)
{
	m_midPos += m_velocity * deltaTime;
	// midPos = calcPosIfLeaving(midPos, colRadius);

	if (IsTooOld(deltaTime))
	{
		m_isDead = true;
	}
}

void Shot::Render()
{
	SDL_FRect rect = GetRenderRect();
	SDL_RenderCopyExF(s_renderer, s_texture, NULL, &rect, m_rotation, NULL, SDL_FLIP_NONE);
}

bool Shot::IsTooOld(float deltaTime)
{
	m_lifeTimeInSeconds += deltaTime;
	return (m_lifeTimeInSeconds > m_maxLifeTimeInSeconds);
}