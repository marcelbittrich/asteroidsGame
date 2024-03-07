#include "initialization.hpp"

#include "../game.hpp"

Vec2 GetFreeRandomPosition(Vec2 windowDimensions, float m_colRadius, const std::vector<GameObject*>& gameObjects)
{
	int maxTries = 10000;
	for (int i = 0; i < maxTries; i++)
	{
		float x = (rand() / (float)RAND_MAX) * windowDimensions.x;
		float y = (rand() / (float)RAND_MAX) * windowDimensions.y;
		bool success = true;
		for (const GameObject* gameObject : gameObjects)
		{
			float distance = sqrtf(std::powf(gameObject->GetMidPos().x - x, 2) + std::powf(gameObject->GetMidPos().y - y, 2));

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
	return Vec2(windowDimensions / 2.f);
}

Vec2 GetRandomVelocity(float minVelocity, float maxVelocity)
{
	Vec2 m_velocity = { 0, 0 };
	m_velocity.x = randomSign() * getRandomValue(minVelocity, maxVelocity);
	m_velocity.y = randomSign() * getRandomValue(minVelocity, maxVelocity);
	return m_velocity;
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
