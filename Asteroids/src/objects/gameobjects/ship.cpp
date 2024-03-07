#include "ship.h"

#include "SDL_timer.h"
#include "SDL_pixels.h"

#include "shot.h"
#include "bomb.h"
#include "../shapes.hpp"
#include "../../game.hpp"
#include "../../audio/audioplayer.hpp"

#ifndef PI
	#define PI 3.14159265359f
#endif

Ship::Ship(Vec2 midPos, int size, SDL_Texture* texture)
	: GameObject(midPos, 0.f)
{
	m_width = size;
	m_height = size;
	m_colRadius = size / 2 * m_colRadiusFactor;

	m_animationCounter = 0;
	m_timeLastShot = SDL_GetTicks();
	m_timeLastBomb = SDL_GetTicks();

	objectType = Type::Ship;
}

void Ship::HandleInput(const InputHandler& myInputHandler)
{
	m_isThrusting = myInputHandler.GetControlBools().giveThrust;
	m_isTurningLeft = myInputHandler.GetControlBools().isTurningLeft;
	m_isTurningRight = myInputHandler.GetControlBools().isTurningRight;

	if ((myInputHandler.GetControlBools()).isShooting) Shoot();
	if ((myInputHandler.GetControlBools()).isUsingBomb) UseBomb();
}

void Ship::Update(float deltaTime)
{
	UpdateGameplay(deltaTime);
	UpdateTransform(deltaTime);
	UpdateAnimation(deltaTime);
}

void Ship::UpdateGameplay(float deltaTime)
{
	if (m_isVisible)
	{
		m_canBomb = true;

		float coolDownFactor = m_canShoot ? 1.0f : 2.0f;
		m_shotCounter = std::max((m_shotCounter - m_shotCounterDecay * coolDownFactor * deltaTime), 0.0f);
		
		if (m_shotCounter >= m_maxShotCounter)
			m_canShoot = false;
		if (!m_canShoot && m_shotCounter <= m_shipCooldownThreshold)
			m_canShoot = true;
	}
	else
	{
		m_canBomb = false;
		m_canShoot = false;
		m_shotCounter = m_maxShotCounter + 1; // turns the UI red
		m_timeNotVisible += deltaTime;
		if (m_timeNotVisible > m_respawnTime)
		{
			m_timeNotVisible = 0;
			m_isVisible = true;
			m_shotCounter = 0;
		}
	}
}

void Ship::UpdateTransform(float deltaTime)
{
	// Update transaltion
	float scalarVelocity = m_velocity.Length();
	float velocityAngle = atan2(m_velocity.x, m_velocity.y);

	scalarVelocity = std::max(scalarVelocity - m_velocityDecay * deltaTime, 0.0f);
	m_velocity.x = (sin(velocityAngle) * scalarVelocity);
	m_velocity.y = (cos(velocityAngle) * scalarVelocity);

	if (m_isThrusting && scalarVelocity < m_velocityMax)
	{
		// TODO: refactor to Vec(1,1) with rotation
		float deltaVelocityX = sinf(m_rotation * (float)PI / 180) * m_thrust * deltaTime;
		float deltaVelocityY = -(cosf(m_rotation * (float)PI / 180)) * m_thrust * deltaTime;
		m_velocity.x += deltaVelocityX;
		m_velocity.y += deltaVelocityY;
	}

	m_midPos += m_velocity * deltaTime;

	// If ship leaves the screen, re-enter at opposite site
	m_midPos = calcPosIfLeavingScreen(m_midPos, 0);

	// Update rotation
	if (m_isTurningRight)
	{
		m_rotation += m_roatatingSpeed * deltaTime;
	}

	if (m_isTurningLeft)
	{
		m_rotation -= m_roatatingSpeed * deltaTime;
	}
}

void Ship::UpdateAnimation(float deltaTime)
{
	if (m_isThrusting)
	{
		if (SDL_GetTicks() - m_timeLastUpdated > m_timeBetweenSprites)
		{
			m_animationCounter++;
			m_animationCounter = m_animationCounter % m_spriteCount + 1;
			m_timeLastUpdated = SDL_GetTicks();
		}
	}
	else
	{
		m_animationCounter = 0;
	}
}

void Ship::Shoot()
{
	Uint32 timeSinceLastShot = SDL_GetTicks() - m_timeLastShot;

	if (m_canShoot && timeSinceLastShot > m_timeBetweenShots && m_shotCounter < m_maxShotCounter)
	{
		CreateShot(0.f);
		s_audioPlayer->PlaySoundEffect(EffectType::ShotSound);
		m_shotCounter = m_shotCounter + 100;

		m_timeLastShot = SDL_GetTicks();
	}
}

void Ship::CreateShot(float additionalRoation)
{
	Vec2 shotVelocityVector = { 0, 0 };

	float rotation = m_rotation + additionalRoation;
	shotVelocityVector.x = sin(rotation / 180 * (float)PI) * m_shotVelocity + m_velocity.x;
	shotVelocityVector.y = -cos(rotation / 180 * (float)PI) * m_shotVelocity + m_velocity.y;

	Vec2 direction = shotVelocityVector;
	Vec2 spawnPoint = m_midPos + direction.Normalize() * (float)m_height / 2.f;
	Shot newShot = Shot(spawnPoint, shotVelocityVector, rotation);
	Game::shots.push_back(newShot);
}

void Ship::UseBomb()
{
	Uint32 timeSinceLastBomb = SDL_GetTicks() - m_timeLastBomb;

	if (!m_collectedBombs.empty() && m_canBomb && timeSinceLastBomb > m_timeBetweenBombs)
	{
		auto iterator = m_collectedBombs.begin();
		Bomb* Bomb = *iterator;

		if (Bomb)
		{
			Bomb->Explode();
		}
		
		m_collectedBombs.erase(iterator);
		m_timeLastBomb = SDL_GetTicks();
	}
}

void Ship::Render()
{
	RenderShotMeter();
	RenderShip();
}

void Ship::RenderShotMeter()
{
	SDL_Color meterColor = m_canShoot ? SDL_Color{ 0, 200, 0, 255 } : SDL_Color{ 200, 0, 0, 255 }; // Green : red

	/// Render meter triangle.
	/// Meter grows starting from the ships nose.
	Vec2 shipNose;
	float noseDistanceToShipMid = m_height * 0.42f;
	shipNose.x = m_midPos.x + SDL_sinf(m_rotation / 180 * (float)PI) * noseDistanceToShipMid;
	shipNose.y = m_midPos.y - SDL_cosf(m_rotation / 180 * (float)PI) * noseDistanceToShipMid;

	/// Render black base layer.
	SDL_Color triangleBaseColor = { 0, 0, 0, 255 };
	float tringleWidth = m_width * 0.5f;
	float trinagleHeight = m_height * 0.625f;
	DrawTriangle(s_renderer, shipNose.x, shipNose.y, tringleWidth, trinagleHeight, m_rotation, triangleBaseColor);

	/// Overlay actual shot meter triangle
	float shotMeterValue = std::min(m_shotCounter / m_maxShotCounter, 1.0f); // from 0 to 1
	if (shotMeterValue > 0.1f)
	{
		DrawTriangle(s_renderer, shipNose.x, shipNose.y, shotMeterValue * tringleWidth, shotMeterValue * trinagleHeight, m_rotation, meterColor);
	}
}

void Ship::RenderShip()
{
	// Grey out texture during respawn
	if (!m_isVisible)
	{
		float timeStepSize = 0.25f;
		int stepValue = (int)(m_timeNotVisible / timeStepSize);
		if (stepValue % 2 == 0)
			SDL_SetTextureColorMod(s_texture, 100, 100, 100);
		else
			SDL_SetTextureColorMod(s_texture, 255, 255, 255);
	}
	else
	{
		SDL_SetTextureColorMod(s_texture, 255, 255, 255);
	}

	int currentSpriteStart = m_spriteWidth * m_animationCounter;
	SDL_Rect srcR{ currentSpriteStart,
				  0,
				  m_spriteWidth,
				  m_spriteHeight };
	SDL_FRect destR = GetRenderRect();

	SDL_RenderCopyExF(s_renderer, s_texture, &srcR, &destR, m_rotation, NULL, SDL_FLIP_NONE);
}

void Ship::CollectBomb(Bomb* bomb)
{
	s_audioPlayer->PlaySoundEffect(EffectType::BombCollectedSound);
	m_collectedBombs.push_back(bomb);
	bomb->GetCollected(this);
}

void Ship::Reset()
{
	int windowWidth, windowHeight;
	SDL_RenderGetLogicalSize(s_renderer, &windowWidth, &windowHeight);

	m_midPos = { windowWidth / 2.0f, windowHeight / 2.0f };
	m_velocity = { 0.0f, 0.0f };
	m_rotation = 0.f;
}

void Ship::Respawn()
{
	s_audioPlayer->PlaySoundEffect(EffectType::ShipDeath);
	Reset();
	m_isVisible = false;
}