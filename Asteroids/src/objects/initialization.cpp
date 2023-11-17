#include "initialization.hpp"

#include "gameobjects/ship.h"

Vec2 GetRandomPosition(
	int windowWidth,
	int windowHeight,
	float m_colRadius,
	const std::list<GameObject*>& gameObjects)
{
	int maxTries = 10000;
	for (int i = 0; i < maxTries; i++)
	{
		float x = rand() % windowWidth;
		float y = rand() % windowHeight;
		bool success = true;
		for (const GameObject* gameObject : gameObjects)
		{
			float distance = sqrtf(std::pow(gameObject->GetMidPos().x - x, 2) + std::pow(gameObject->GetMidPos().y - y, 2));

			float objectColRadius;
			(gameObject->GetID() == 1) ? objectColRadius = 200 : objectColRadius = gameObject->GetColRadius();

			if (distance < objectColRadius + m_colRadius)
			{
				// std::cout << "RandomPosTry: " << i + 1 << std::endl;
				success = false;
				break;
			}
		}
		if (success)
		{
			Vec2 point = { x, y };
			return point;
		}
	}
	//throw std::runtime_error("Max tries for getRandomPosition exceeded!");
}

float randomSign()
{
	if (rand() % 100 <= 49)
		return -1.0f;
	return 1.0f;
}

float getRandomValue(float min, float max)
{
	float randomValue = (rand() % 1001) / 1000.0f * (max - min);
	return randomValue;
}

Vec2 GetRandomVelocity(float minVelocity, float maxVelocity)
{
	Vec2 m_velocity = { 0, 0 };
	m_velocity.x = randomSign() * getRandomValue(minVelocity, maxVelocity);
	m_velocity.y = randomSign() * getRandomValue(minVelocity, maxVelocity);
	return m_velocity;
}

Asteroid InitSingleAsteroid(std::list<GameObject*>& gameObjects, int windowWidth, int windowHeight, Asteroid::SizeType sizeType)
{
	float asteroidMinVel = 0;
	float asteroidMaxVel = 1;
	int size = Asteroid::GetSize(sizeType);
	float m_colRadius = Asteroid::GetColRadius(size);
	Vec2 randomPosition = GetRandomPosition(
		windowWidth, windowHeight, m_colRadius, gameObjects);

	return Asteroid(randomPosition, GetRandomVelocity(asteroidMinVel, asteroidMaxVel), sizeType);
}

void InitAsteroids(int windowWidth, int windowHeight)
{
	int asteroidAmountSmall = 5;
	int asteroidAmountMedium = 5;
	Ship ship;
	std::list<GameObject*> gameObjects = { &Ship::ships[0] }; // TODO: refactor with all gameObjects
	for (int i = 0; i < asteroidAmountSmall; i++)
	{
		Asteroid newAsteroid = InitSingleAsteroid(gameObjects, windowWidth, windowHeight, Asteroid::SizeType::Small);
	}
	for (int i = 0; i < asteroidAmountMedium; i++)
	{
		Asteroid newAsteroid = InitSingleAsteroid(gameObjects, windowWidth, windowHeight, Asteroid::SizeType::Medium);
	}
}
