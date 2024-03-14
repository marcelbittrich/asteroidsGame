#pragma once

#include "SDL_stdinc.h"

#include "gameobject.hpp"

class Follower : public Enemy
{
public:
	Follower(Vec2 midPos, int size);
		
	void Update(float deltaTime) override;
	void Render() override;

	void HandleDestruction() override {};

	static void SetTexture(struct SDL_Texture* texture) {
		s_texture = texture;
	}
private:
	inline static struct SDL_Texture* s_texture;

	// Animation values
	int m_spriteWidth = 300;
	int m_spriteHeight = 300;
	int m_spriteCount = 3;	// Number of different sprites in texture
	int m_animationCounter = 0;	// Used to select sprite
	Uint32 m_timeBetweenSprites = 300;
	Uint32 m_timeLastUpdated = 0;

	// Gameplay Values
	float m_velocityMax = 50.f;
	float m_velocityDecay = 0.5f;
	float m_followSpeed = 50.f;
	float m_backOfSpeed = 150.f;

	void UpdateTransform(float deltaTime);
	void UpdateAnimation(float deltaTime);
};

