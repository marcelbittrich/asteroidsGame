#pragma once

#include <vector>
#include <list>

#include "SDL_stdinc.h"

#include "gameobject.hpp"
#include "../../input/inputhandler.hpp"

class Ship : public GameObject
{
public:
	Ship() {};
	Ship(Vec2 midPos, int size, struct SDL_Texture* texture);

	void HandleInput(const InputHandler& myInputHandler);
	void Update(float deltaTime) override;
	void Render() override;
	void Reset();
	void Respawn();
	void CollectBomb(class Bomb* bomb);

	float GetMaxVelocity() const { return m_velocityMax; };
	float GetShotCounter() const { return m_shotCounter; };
	float GetMaxShotCounter() const { return m_maxShotCounter; };
	bool GetCanShoot() const { return m_canShoot; };
	float GetShotVelocity() const { return m_shotVelocity; };
	int GetCollectedBombsSize() const { return (int)m_collectedBombs.size(); };

	static void SetTexture(struct SDL_Texture* texture) { s_texture = texture; }

private:
	inline static struct SDL_Texture* s_texture;
	// Update shooting capability and ship visibility.
	// Ship is not visible during respawn.
	float m_respawnTime = 3.f;
	float m_timeNotVisible = 0.f;
	// Collision values
	float m_colRadiusFactor = 0.6f;

	// Movement values
	float m_velocityMax = 1000.f;
	float m_velocityDecay = 50.f;
	float m_rotation = 0.f;
	float m_roatatingSpeed = 180.f;
	float m_thrust = 350.f;
	bool m_isThrusting = false;
	bool m_isTurningRight = false;
	bool m_isTurningLeft = false;

	// Shooting values
	float m_shotVelocity = 1000.f;
	float m_shotCounter = 0.0f;
	float m_shotCounterDecay = 150.0f;
	float m_maxShotCounter = 1000.0f;
	float m_shipCooldownThreshold = m_maxShotCounter / 2.f;
	bool m_hasShotPowerUp = false;

	bool m_canShoot = true;	            // Indicator, false during respawn.
	Uint32 m_timeLastShot = 0;
	Uint32 m_timeBetweenShots = 250;

	// Bomb values
	std::list<class Bomb*> m_collectedBombs;
	bool m_canBomb = true;	            // Indicator, false during respawn.
	Uint32 m_timeLastBomb = 0;
	Uint32 m_timeBetweenBombs = 250;

	// Animation values
	int m_spriteWidth = 300;
	int m_spriteHeight = 300;
	int m_spriteCount = 3;	            // Number of different sprites in texture
	int m_animationCounter = 0;	        // Used to select sprite
	Uint32 m_timeBetweenSprites = 300;
	Uint32 m_timeLastUpdated = 0;

	void UpdateGameplay(float deltaTime);
	void UpdateTransform(float deltaTime);
	void UpdateAnimation(float deltaTime);
	void Shoot();
	void CreateShot(float additionalRoation);
	void UseBomb();
	void RenderShotMeter();
	void RenderShip();
};