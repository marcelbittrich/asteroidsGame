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

Ship::Ship(Vec2 midPos, int size)
	: GameObject(midPos, 0.f)
{
	m_width = size;
	m_height = size;
	m_colRadius = size / 2 * m_colRadiusFactor;

	m_animationCounter = 0;
	m_timeLastShot = SDL_GetTicks();
	m_timeLastBomb = SDL_GetTicks();

	objectType = Type::Ship;
	colType = CollisionType::Ship;
}

void Ship::HandleInput(const InputHandler& myInputHandler)
{
	m_isThrusting = myInputHandler.GetControlBools().giveThrust;

	m_isTurningLeft = myInputHandler.GetControlBools().isTurningLeft;
	m_isTurningRight = myInputHandler.GetControlBools().isTurningRight;

	float digitalInput = 0.f;
	if (m_isTurningLeft)
	{
		digitalInput += -1.f;
	}
	if (m_isTurningRight)
	{
		digitalInput += 1.f;
	}
	
	float analogInput = myInputHandler.GetControllerAnlogInput().LeftStick.x;
	m_turnInput = std::abs(analogInput) > 0.f ? analogInput : digitalInput;

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
		m_canShoot = true;

		if (m_hasShotPowerUp)
		{
			m_currentPowerUpTime += deltaTime;
			m_shotMeterValue = 1.f - m_currentPowerUpTime / m_maxPowerUpTime;
			if (m_currentPowerUpTime > m_maxPowerUpTime)
				m_hasShotPowerUp = false;
		}
	}
	else
	{
		m_canBomb = false;
		m_canShoot = false;
		m_shotMeterValue = 1.f;
		m_timeNotVisible += deltaTime;
		if (m_timeNotVisible > m_respawnTime)
		{
			m_timeNotVisible = 0;
			m_isVisible = true;
			m_shotMeterValue = 0.f;
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
		Vec2 thrustVector(0, -1);
		thrustVector = thrustVector.Rotate(m_rotation) * m_thrust * deltaTime;
		m_velocity += thrustVector;
	}

	m_midPos += m_velocity * deltaTime;

	// If ship leaves the screen, re-enter at opposite site
	m_midPos = calcPosIfLeavingScreen(m_midPos, 0);

	m_rotation += m_roatatingSpeed * m_turnInput * deltaTime;
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

	if (m_canShoot && timeSinceLastShot > m_timeBetweenShots)
	{
		if (m_hasShotPowerUp)
		{
			CreateShot(-5.f);
			CreateShot(0.f);
			CreateShot(5.f);
		}
		else
		{
			CreateShot(0.f);
		}

		s_audioPlayer->PlaySoundEffect(EffectType::ShotSound);
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
	const SDL_Color green = { 0, 200, 0, 255 };
	const SDL_Color red = { 200, 0, 0, 255 };
	SDL_Color meterColor = m_canShoot ? green : red;

	/// Render meter triangle.
	/// Meter size starting from the ships nose.
	Vec2 noseDistanceToShipMid = { 0, -m_height * 0.42f };
	Vec2 shipNosePosition = m_midPos + noseDistanceToShipMid.Rotate(m_rotation);

	/// Render black base layer.
	SDL_Color triangleBaseColor = { 0, 0, 0, 255 };
	float tringleWidth = m_width * 0.5f;
	float trinagleHeight = m_height * 0.625f;
	DrawTriangle(s_renderer, shipNosePosition, tringleWidth, trinagleHeight, m_rotation, triangleBaseColor);

	if (m_shotMeterValue > 0.1f)
	{
		DrawTriangle(s_renderer, shipNosePosition, m_shotMeterValue * tringleWidth, m_shotMeterValue * trinagleHeight, m_rotation, meterColor);
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
	m_collectedBombs.push_back(bomb);
}

void Ship::CollectPowerUp()
{
	m_currentPowerUpTime = 0.f;
	m_hasShotPowerUp = true;
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