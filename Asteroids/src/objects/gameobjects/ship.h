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
	void Update(int windowWidth, int windowHeight, float deltaTime) override;
	void Render() override;
	void Reset();
	void Respawn();
	void CollectBomb(class Bomb* bomb);

	float GetMaxVelocity() { return m_velocityMax; };
	float GetShotCounter() { return m_shotCounter; };
	float GetMaxShotCounter() { return m_maxShotCounter; };
	bool GetCanShoot() { return m_canShoot; };
	float GetShotVelocity() { return m_shotVelocity; };
	int GetCollectedBombsSize() { return (int)m_collectedBombs.size(); };

	static void SetTexture(struct SDL_Texture* texture) { s_texture = texture; }

	// TODO: Let the Game own this
	inline static std::vector<Ship> ships;

private:
	inline static struct SDL_Texture* s_texture;
	// Update shooting capability and ship visibility.
	// Ship is not visible during respawn.
	float m_respawnTime = 3.f;
	float m_timeNotVisible = 0.f;
	void UpdateVisibility(float deltaTime);

	// movement values
	float m_velocityMax = 1000.f;
	float m_velocityDecay = 50.f;
	float m_rotation = 0.f;
	float m_roatatingSpeed = 180.f;
	float m_thrust = 350.f;
	bool m_isThrusting = false;
	bool m_isTurningRight = false;
	bool m_isTurningLeft = false;
	void UpdateTransform(int windowWidth, int windowHeight, float deltaTime);
	void UpdateAnimation(float deltaTime);

	// Shooting values
	float m_shotVelocity = 1000.f;
	float m_shotCounter = 0.0f;
	float m_shotCounterDecay = 150.0f;
	float m_maxShotCounter = 1000.0f;
	float m_shipCooldownThreshold = m_maxShotCounter / 2.f;

	bool m_canShoot = true;	// Indicator, false during respawn.
	Uint32 m_timeLastShot = 0;
	Uint32 m_timeBetweenShots = 250;
	void Shoot();
	void CreateShot(float additionalRoation);

	// Bombing values
	std::list<class Bomb*> m_collectedBombs;

	bool m_canBomb = true;	// Indicator, false during respawn.
	Uint32 m_timeLastBomb = 0;
	Uint32 m_timeBetweenBombs = 250;
	void UseBomb();

	// Animation values
	int m_spriteWidth = 300;
	int m_spriteHeight = 300;
	int m_spriteCount = 3;	// Number of different sprites in texture
	int m_animationCounter = 0;	// Used to select sprite
	Uint32 m_timeBetweenSprites = 300;
	Uint32 m_timeLastUpdated = 0;
	void RenderShotMeter();
	void RenderShip();

	// collision values
	float m_colRadiusFactor = 0.6f;
};