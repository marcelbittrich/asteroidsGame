#include "asteroid.h"

#include "SDL_rect.h"
#include "SDL_render.h"

#include "../../audio/audioplayer.hpp"
#include "../../physics/collisionhandler.h"

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

void spawnAsteroid(Vec2 position, Vec2 velocity, Asteroid::SizeType sizeType, const std::list<GameObject*>& gameobjects)
{
	GameObject collisionObject = GameObject();
	collisionObject.SetMidPos(position);
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