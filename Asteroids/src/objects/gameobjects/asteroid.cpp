#include "asteroid.h"

#include "SDL_rect.h"
#include "SDL_render.h"

#include "../helper.hpp"
#include "../../game.hpp"
#include "../../audio/audioplayer.hpp"
#include "../../physics/collisionhandler.h"

Asteroid::Asteroid(Vec2 midPos, Vec2 velocity, Asteroid::SizeType sizeType)
	: Enemy(midPos, velocity), sizeType(sizeType)
{
	int size = GetSize(sizeType);
	m_colRadius = GetColRadius(size);
	m_width = size;
	m_height = size;

	objectType = Type::Asteroid;
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

void Asteroid::Update(float deltaTime)
{
	if (m_isVisible)
	{
		m_midPos += m_velocity * deltaTime;
	}

	Vec2 newMidPosistion = calcPosIfLeavingScreen(m_midPos, m_colRadius);

	if ((m_midPos.x != newMidPosistion.x) || (m_midPos.y != newMidPosistion.y))
	{
		m_isVisible = false;
	}

	if (!m_isVisible)
	{
		m_isVisible = true;
		bool canStayVisible = true;
		for (const Asteroid& otherAsteroid : Game::asteroids)
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
	if (sizeType == Asteroid::SizeType::Medium)
	{
		s_audioPlayer->PlaySoundEffect(EffectType::BigAsteroidExplode);

		int newAsteroidSize = Asteroid::GetSize(Asteroid::SizeType::Small);

		Vec2 spawnDirection = m_velocity.Rotate(90);
		spawnDirection.SetLength(newAsteroidSize / 2.f);

		Asteroid newAsteroid = Asteroid(
			m_midPos + spawnDirection,
			m_velocity.Rotate(45) * m_DestAstroidVelFactor,
			Asteroid::SizeType::Small);

		Game::asteroids.push_back(newAsteroid);

		newAsteroid = Asteroid(
			m_midPos - spawnDirection,
			m_velocity.Rotate(-45) * m_DestAstroidVelFactor,
			Asteroid::SizeType::Small);

		Game::asteroids.push_back(newAsteroid);
	}
	else
	{
		s_audioPlayer->PlaySoundEffect(EffectType::SmallAsteroidExplode);
	}
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

//Asteroid spawnAsteroid(Vec2 position, Vec2 velocity, Asteroid::SizeType sizeType)
//{
//	GameObject collisionObject = GameObject();
//	collisionObject.SetMidPos(position);
//	collisionObject.SetColRadius(Asteroid::GetColRadius(Asteroid::GetSize(sizeType)));
//
//	return Asteroid(collisionObject.GetMidPos(),velocity,sizeType);
//}


std::vector<Asteroid> InitAsteroids(int amnountSmall, int amountBig, Vec2 windowDimensions)
{
	std::vector<Asteroid> asteroids;
	asteroids.reserve(amountBig + amnountSmall);

	std::vector<GameObject*> currentGameObjects = { &Game::ships[0] };
	for (int i = 0; i < amnountSmall; i++)
	{
		Asteroid newAsteroid = InitSingleAsteroid(Asteroid::SizeType::Small, windowDimensions, currentGameObjects);
		asteroids.push_back(newAsteroid);
		currentGameObjects.push_back(&asteroids.back());
	}
	for (int i = 0; i < amountBig; i++)
	{
		Asteroid newAsteroid = InitSingleAsteroid(Asteroid::SizeType::Medium, windowDimensions, currentGameObjects);
		asteroids.push_back(newAsteroid);
		currentGameObjects.push_back(&asteroids.back());
	}

	return asteroids;
}

Asteroid InitSingleAsteroid(Asteroid::SizeType sizeType, Vec2 windowDimensions, std::vector<GameObject*>& gameObjects)
{
	float asteroidMinVel = 0;
	float asteroidMaxVel = 60;
	int size = Asteroid::GetSize(sizeType);
	float m_colRadius = Asteroid::GetColRadius(size);
	Vec2 randomPosition = GetFreeRandomPosition(windowDimensions, m_colRadius, gameObjects);
	return Asteroid(randomPosition, GetRandomVelocity(asteroidMinVel, asteroidMaxVel), sizeType);
}
