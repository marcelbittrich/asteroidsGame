#include "initialization.hpp"

Vec2 getRandomPosition(
	int windowWidth,
	int windowHeight,
	float m_colRadius,
	const std::list<GameObject>& gameObjects)
{
	int maxTries = 10000;
	for (int i = 0; i < maxTries; i++)
	{
		float x = rand() % windowWidth;
		float y = rand() % windowHeight;
		bool success = true;
		for (const GameObject& gameObject : gameObjects)
		{
			float distance = sqrtf(std::pow(gameObject.getMidPos().x - x, 2) + std::pow(gameObject.getMidPos().y - y, 2));

			float objectColRadius;
			(gameObject.getID() == 1) ? objectColRadius = 200 : objectColRadius = gameObject.getColRadius();

			if (distance < objectColRadius + m_colRadius)
			{
				std::cout << "RandomPosTry: " << i + 1 << std::endl;
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
	throw std::runtime_error("Max tries for getRandomPosition exceeded!");
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

Vec2 getRandomVelocity(float minVelocity, float maxVelocity)
{
	Vec2 m_velocity = { 0, 0 };
	m_velocity.x = randomSign() * getRandomValue(minVelocity, maxVelocity);
	m_velocity.y = randomSign() * getRandomValue(minVelocity, maxVelocity);
	return m_velocity;
}

void initSingleAsteroid(std::list<GameObject>& gameObjects, int windowWidth, int windowHeight, AsteroidSizeType sizeType)
{
	float asteroidMinVel = 0;
	float asteroidMaxVel = 1;
	int size = Asteroid::getSize(sizeType);
	float m_colRadius = Asteroid::getColRadius(size);
	Vec2 randomPosition = getRandomPosition(
		windowWidth, windowHeight, m_colRadius, gameObjects);
	Asteroid(randomPosition, getRandomVelocity(asteroidMinVel, asteroidMaxVel), sizeType);
}

void initAsteroids(GameObject ship, int windowWidth, int windowHeight)
{
	int asteroidAmountSmall = 5;
	int asteroidAmountMedium = 5;
	std::list<GameObject> gameObjects = { ship };
	for (int i = 0; i < asteroidAmountSmall; i++)
	{
		initSingleAsteroid(gameObjects, windowWidth, windowHeight, AsteroidSizeType::Small);
	}
	for (int i = 0; i < asteroidAmountMedium; i++)
	{
		initSingleAsteroid(gameObjects, windowWidth, windowHeight, AsteroidSizeType::Medium);
	}
}
