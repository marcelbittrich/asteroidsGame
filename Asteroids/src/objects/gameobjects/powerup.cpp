#include "powerup.h"

#include "SDL_rect.h"
#include "SDL_render.h"
#include "ship.h"

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
	m_midPos.x += m_velocity.x * deltaTime * 60;
	m_midPos.y += m_velocity.y * deltaTime * 60;
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
	std::cout << "I was collected! " << std::endl;
	ownerShip->CollectPowerUp();
	m_isDead = true;
}
