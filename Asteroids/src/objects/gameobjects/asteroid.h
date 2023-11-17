#pragma once

#include <list>

#include "gameobject.hpp"

class Asteroid : public GameObject
{
public:
	enum class SizeType
	{
		Small,
		Medium
	};

	Asteroid(Vec2 m_midPos, Vec2 m_velocity, Asteroid::SizeType sizeType);
	void Update(int windowWidth, int windowHeight, float deltaTime)  override;
	void Render() override;

	SizeType sizeType;

	static int GetSize(SizeType sizeType);
	static float GetColRadius(int size);

	static void SetTextureSmall(struct SDL_Texture* texture) { s_textureSmall = texture; }
	static void SetTextureMedium(struct SDL_Texture* texture) { s_textureMedium = texture; }

	void HandleDestruction();

	// TODO: Let the Game own this
	inline static std::list<Asteroid> asteroids;

private:
	inline static const float m_colRadiusFactor = 0.6f;
	const float m_DestAstroidVelFactor = 2.0;
	static const int m_sizeSmall = 50;
	static const int m_sizeMedium = 100;
	inline static SDL_Texture* s_textureSmall;
	inline static SDL_Texture* s_textureMedium;
};

void spawnAsteroid(Vec2 position, Vec2 velocity, Asteroid::SizeType sizeType, const std::list<GameObject*>& gameobjects);

