#include "gameobjects.hpp"

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>
#include "collisionhandler.h"

#define PI 3.14159265359f

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

	ships.push_back(*this);
}

void Ship::Update(const InputHandler& myInputHandler, int windowWidth, int windowHeight, float deltaTime)
{
	UpdateVisibility(deltaTime);
	UpdateTransform(myInputHandler, windowWidth, windowHeight, deltaTime);
	UpdateAnimation(myInputHandler, deltaTime);

	if ((myInputHandler.GetControlBools()).isShooting)
		Shoot();

	if ((myInputHandler.GetControlBools()).isUsingBomb)
		UseBomb();
}

void Ship::UpdateVisibility(float deltaTime)
{
	if (m_isVisible)
	{
		m_canBomb = true;
		m_shotCounter = std::max((m_shotCounter - m_shotCounterDecay * deltaTime), 0.0f);
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

void Ship::UpdateTransform(const InputHandler& myInputHandler, int windowWidth, int windowHeight, float deltaTime)
{
	ControlBools CurrentControlBools = myInputHandler.GetControlBools();

	// Update transaltion
	float scalarVelocity = m_velocity.Length();
	float velocityAngle = atan2(m_velocity.x, m_velocity.y);

	scalarVelocity = std::max(scalarVelocity - m_velocityDecay * deltaTime, 0.0f);
	m_velocity.x = (sin(velocityAngle) * scalarVelocity);
	m_velocity.y = (cos(velocityAngle) * scalarVelocity);

	if (CurrentControlBools.giveThrust && scalarVelocity < m_velocityMax)
	{
		// TODO: refactor to Vec(1,1) with rotation
		float deltaVelocityX = sinf(m_rotation * PI / 180) * m_thrust * deltaTime;
		float deltaVelocityY = -(cosf(m_rotation * PI / 180)) * m_thrust * deltaTime;
		m_velocity.x += deltaVelocityX;
		m_velocity.y += deltaVelocityY;
	}

	m_midPos += m_velocity * deltaTime;

	// If ship leaves the screen, re-enter at opposite site
	m_midPos = calcPosIfLeaving(m_midPos, 0, windowWidth, windowHeight);

	// Update rotation
	if (CurrentControlBools.isTurningRight)
	{
		m_rotation += m_roatatingSpeed * deltaTime;
	}

	if (CurrentControlBools.isTurningLeft)
	{
		m_rotation -= m_roatatingSpeed * deltaTime;
	}
}

void Ship::UpdateAnimation(const InputHandler& myInputHandler, float deltaTime)
{
	ControlBools CurrentControlBools = myInputHandler.GetControlBools();
	if (CurrentControlBools.giveThrust)
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
		CreateShot();
		s_audioPlayer->PlaySoundEffect(EffectType::ShotSound);
		m_shotCounter = m_shotCounter + 100;

		m_timeLastShot = SDL_GetTicks();
	}
}

void Ship::CreateShot()
{
	Vec2 shotVelocityVector = { 0, 0 };

	shotVelocityVector.x = sin(m_rotation / 180 * PI) * m_shotVelocity + m_velocity.x;
	shotVelocityVector.y = -cos(m_rotation / 180 * PI) * m_shotVelocity + m_velocity.y;

	Vec2 direction = shotVelocityVector;
	Vec2 spawnPoint = m_midPos + direction.Normalize() * m_height / 2.f;
	Shot(spawnPoint, shotVelocityVector, m_rotation);
}

void Ship::UseBomb()
{
	Uint32 timeSinceLastBomb = SDL_GetTicks() - m_timeLastBomb;

	if (!m_collectedBombs.empty() && m_canBomb && timeSinceLastBomb > m_timeBetweenBombs)
	{
		auto iterator = m_collectedBombs.begin();
		Bomb* Bomb = *iterator;

		assert(Bomb);
		Bomb->Explode();

		m_collectedBombs.erase(iterator);

		m_timeLastBomb = SDL_GetTicks();
	}
}

void Ship::Render()
{
	assert(s_renderer && s_texture);

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
	shipNose.x = m_midPos.x + SDL_sinf(m_rotation / 180 * PI) * noseDistanceToShipMid;
	shipNose.y = m_midPos.y - SDL_cosf(m_rotation / 180 * PI) * noseDistanceToShipMid;

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
		int stepValue = floor(m_timeNotVisible / timeStepSize);
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

Asteroid::Asteroid(Vec2 midPos, Vec2 velocity, Asteroid::SizeType sizeType) 
	: GameObject(midPos, velocity), sizeType(sizeType)
{
	int size = GetSize(sizeType);
	m_colRadius = GetColRadius(size);
	m_width = size;
	m_height = size;

	objectType = Type::Asteroid;

	asteroids.push_back(*this);
}

int Asteroid::GetSize(Asteroid::SizeType sizeType)
{
	int size = -1;
	if (sizeType == Asteroid::SizeType::Small)
		size = m_sizeSmall;
	else if (sizeType == Asteroid::SizeType::Medium)
		size = m_sizeMedium;
	return size;
}

float Asteroid::GetColRadius(int size)
{
	return size / 2.f * m_colRadiusFactor;
}

void Asteroid::Update(int windowWidth, int windowHeight, float deltaTime)
{
	if (m_isVisible)
	{
		m_midPos.x += m_velocity.x * deltaTime * 60;
		m_midPos.y += m_velocity.y * deltaTime * 60;
	}

	Vec2 newMidPosistion = calcPosIfLeaving(m_midPos, m_colRadius, windowWidth, windowHeight);

	if ((m_midPos.x != newMidPosistion.x) || (m_midPos.y != newMidPosistion.y))
	{
		m_isVisible = false;
	}

	if (!m_isVisible)
	{
		m_isVisible = true;
		bool canStayVisible = true;
		for (const Asteroid& otherAsteroid : Asteroid::asteroids)
		{
			if (m_id == otherAsteroid.m_id)
				continue;
			if (CollisionHandler::DoesCollide(*this, otherAsteroid))
			{
				canStayVisible = false;
				break;
			}
		}
		if (!canStayVisible)
		{
			m_isVisible = false;
		}
	}

	m_midPos = newMidPosistion;
}

void Asteroid::HandleDestruction()
{
	s_audioPlayer->PlaySoundEffect(EffectType::BigAsteroidExplode);

	int newAsteroidSize = Asteroid::GetSize(Asteroid::SizeType::Small);

	Vec2 spawnDirection = m_velocity.Rotate(90);
	spawnDirection.SetLength(newAsteroidSize / 2.f);

	Asteroid(
		m_midPos + spawnDirection,
		m_velocity.Rotate(45) * m_DestAstroidVelFactor,
		Asteroid::SizeType::Small);

	Asteroid(
		m_midPos - spawnDirection,
		m_velocity.Rotate(-45) * m_DestAstroidVelFactor,
		Asteroid::SizeType::Small);
}

void Asteroid::Render()
{
	SDL_Texture* asteroidTex = nullptr;
	if (sizeType == Asteroid::SizeType::Medium)
	{
		asteroidTex = s_textureMedium;
	}
	else if (sizeType == Asteroid::SizeType::Small)
	{
		asteroidTex = s_textureSmall;
	}
	else
	{
		throw std::runtime_error("Unknown AsteroidSizeType for rendering");
	}
	if (m_isVisible)
	{
		SDL_FRect asteroidRect = GetRenderRect();
		SDL_RenderCopyExF(s_renderer, asteroidTex, NULL, &asteroidRect, 0.0f, NULL, SDL_FLIP_NONE);
	}
}

Shot::Shot(Vec2 midPos, Vec2 velocity, float shotHeadingAngle)
	: GameObject(midPos, velocity)
{
	m_creationTime = SDL_GetTicks();
	m_rotation = shotHeadingAngle;

	m_colRadius = m_size * m_colRadiusFactor;

	m_width = m_size;
	m_height = m_size;

	objectType = Type::Shot;

	shots.push_back(*this);
}

void Shot::Update(int windowWidth, int windowHeight, float deltaTime)
{
	m_midPos.x += m_velocity.x * deltaTime;
	m_midPos.y += m_velocity.y * deltaTime;
	// midPos = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);

	if (TooOld())
	{
		m_isDead = true;
	}
}

void Shot::Render()
{
	SDL_FRect rect = GetRenderRect();
	SDL_RenderCopyExF(s_renderer, s_texture, NULL, &rect, m_rotation, NULL, SDL_FLIP_NONE);
}

bool Shot::TooOld()
{
	Uint32 deltaTime = SDL_GetTicks() - m_creationTime;
	return (m_maxLifeTime < deltaTime);
}

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

void spawnAsteroid(int xPos, int yPos, Vec2 velocity, Asteroid::SizeType sizeType, const std::list<GameObject*>& gameobjects)
{
	GameObject collisionObject = GameObject();
	collisionObject.SetMidPos((float)xPos, (float)yPos);
	collisionObject.SetColRadius(Asteroid::GetColRadius(Asteroid::GetSize(sizeType)));

	bool isSafeToSpawn = true;
	for (auto it = gameobjects.begin(); it != gameobjects.end(); it++)
	{
		if (CollisionHandler::DoesCollide(collisionObject, **it))
		{
			isSafeToSpawn = false;
			break;
		}
	}
	if (isSafeToSpawn)
	{
		Asteroid(
			collisionObject.GetMidPos(),
			velocity,
			sizeType);
	}
}

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
