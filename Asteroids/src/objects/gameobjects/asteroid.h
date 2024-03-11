#pragma once

#include <vector>

#include "gameobject.hpp"

class Asteroid : public Enemy
{
public:
	enum class SizeType
	{
		Small,
		Medium
	};

	Asteroid(Vec2 m_midPos, Vec2 m_velocity, Asteroid::SizeType sizeType);
	void Update(float deltaTime)  override;
	void Render() override;

	SizeType sizeType;

	static int GetSize(SizeType sizeType);
	static float GetColRadius(int size);

	static void SetTextureSmall(struct SDL_Texture* texture) { s_textureSmall = texture; }
	static void SetTextureMedium(struct SDL_Texture* texture) { s_textureMedium = texture; }

	void HandleDestruction() override;

private:
	inline static const float m_colRadiusFactor = 0.6f;
	float m_DestAstroidVelFactor = 2.0;
	static const int m_sizeSmall = 50;
	static const int m_sizeMedium = 100;
	inline static SDL_Texture* s_textureSmall;
	inline static SDL_Texture* s_textureMedium;
};

//Asteroid spawnAsteroid(Vec2 position, Vec2 velocity, Asteroid::SizeType sizeType);

std::vector<Asteroid> InitAsteroids(int amnountSmall, int amountBig, Vec2 windowDimensions);
Asteroid InitSingleAsteroid(Asteroid::SizeType sizeType, Vec2 windowDimensions, std::vector<GameObject*>& gameObjects);
