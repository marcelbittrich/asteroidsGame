#pragma once

#include <iostream>
#include <math.h>
#include <vector>
#include <list>
#include <stdexcept>

#include "inputhandler.hpp"
#include "shapes.hpp"
#include "vector2.hpp"

class GameObject
{
public:
	GameObject() : m_id(s_NewId++) {}
	GameObject(Vec2 midPos, Vec2 velocity) 
		: m_id(s_NewId++), m_midPos(midPos), m_velocity(velocity) {}

	static void ResetId() { s_NewId = 1; }
	virtual void Update() {};
	virtual void Render(SDL_Renderer* renderer) {};

	enum class Type {
		Default,
		Ship,
		AsteroidSmall,
		AsteroidMedium,
		Bomb,
		Shot
	};

	Type objectType = Type::Default;

	int GetID() const { return m_id; };
	int GetWidth() const { return m_width; };
	int GetHeight() const { return m_height; };
	float GetColRadius() const { return m_colRadius; };
	bool GetVisibility() const { return m_isVisible; };
	bool GetIsDead() const { return m_isDead; };
	Vec2 GetMidPos() const { return m_midPos; };
	Vec2 GetVelocity() const { return m_velocity; };

	void SetColRadius(float radius) { m_colRadius = radius; };
	void SetMidPos(float x, float y) { m_midPos = { x, y }; };
	void SetVelocity(float x, float y) { m_velocity = { x, y }; };

protected:
	int m_id			= -1;
	int m_width			= 0;
	int m_height		= 0;
	float m_colRadius	= 0.f;
	bool m_isVisible	= true;		// Indicator, false during respawn.
	bool m_isDead		= false;	// Can be destroyed.

	Vec2 m_midPos;
	Vec2 m_velocity;

	SDL_Rect getRenderRect() const;

private:
	inline static int s_NewId = 0;
};

class Ship : public GameObject
{
public:
	Ship();
	Ship(Vec2 midPos, int size, SDL_Texture* texture);

	void Update(const InputHandler& MyInputHandler, int windowWidth, int windowHeight, float deltaTime);
	void Render(SDL_Renderer* renderer) override;
	void Reset(SDL_Renderer* renderer);
	void Respawn(SDL_Renderer* renderer);

	static void SetTexture(SDL_Texture* texture) { s_texture = texture; }

private:
	inline static SDL_Texture* s_texture;
	// Update shooting capability and ship visibility.
	// Ship is not visible during respawn.
	float m_respawnTime			= 3.f;
	float m_timeNotVisible		= 0.f;
	void updateVisibility(float deltaTime);

	// movement values
	float m_velocityMax			= 1000.f;
	float m_velocityDecay		= 50.f;
	float m_rotation			= 0.f;
	float m_roatatingSpeed		= 180.f;
	float m_thrust				= 350.f;
	void updateTransform(const InputHandler& MyInputHandler, int windowWidth, int windowHeight, float deltaTime);
	void updateAnimation(const InputHandler& MyInputHandler, float deltaTime);

	// Shooting values
	float m_shotVelocity		= 1000.f;
	float m_shotCounter			= 0.0f;
	float m_shotCounterDecay	= 150.0f;
	float m_maxShotCounter		= 1000.0f;
	float m_shipCooldownThreshold = m_maxShotCounter / 2.f;

	bool m_canShoot				= true;	// Indicator, false during respawn.
	Uint32 m_timeLastShot		= 0;
	Uint32 m_timeBetweenShots	= 250;
	void shoot();
	void createShot();

	// Bombing values
	std::list<class Bomb*> m_collectedBombs;

	bool m_canBomb				= true;	// Indicator, false during respawn.
	Uint32 m_timeLastBomb		= 0;
	Uint32 m_timeBetweenBombs	= 250;
	void useBomb();

	// Animation values
	int m_spriteWidth			= 300;
	int m_spriteHeight			= 300;
	int m_spriteCount			= 3;	// Number of different sprites in texture
	int m_animationCounter		= 0;	// Used to select sprite
	Uint32 m_timeBetweenSprites = 300;
	Uint32 m_timeLastUpdated	= 0;
	void renderShotMeter(SDL_Renderer* renderer);
	void renderShip(SDL_Renderer* renderer);

	// collision values
	float sizeToCollisonRadiusRatio = 0.6f;

public:
	float getMaxVelocity() { return m_velocityMax; };
	float getShotCounter() { return m_shotCounter; };
	float getMaxShotCounter() { return m_maxShotCounter; };
	bool getCanShoot() { return m_canShoot; };
	float getShotVelocity() { return m_shotVelocity; };
	void collectBomb(class Bomb* bomb);
	int getCollectedBombsSize() { return (int)m_collectedBombs.size(); };
};

enum class AsteroidSizeType
{
	Small,
	Medium
};

class Asteroid : public GameObject
{
public:
	Asteroid(Vec2 m_midPos, Vec2 m_velocity, AsteroidSizeType sizeType);
	void Update(int windowWidth, int windowHeight, float deltaTime);
	void Render(SDL_Renderer* renderer) override;

	AsteroidSizeType sizeType;

	static std::list<Asteroid> asteroids;
	static int getSize(AsteroidSizeType sizeType);
	static float GetColRadius(int size);

	static void setTextureSmall(SDL_Texture* texture) { s_textureSmall = texture; }
	static void setTextureMedium(SDL_Texture* texture) { s_textureMedium = texture; }

	void handleDestruction();

private:
	inline static const float m_colRadiusFactor = 0.6f;
	const float m_DestAstroidVelFactor			= 2.0;
	static const int m_sizeSmall				= 50;
	static const int m_sizeMedium				= 100;
	inline static SDL_Texture* s_textureSmall;
	inline static SDL_Texture* s_textureMedium;
};


bool doesCollide(const GameObject& firstObject, const GameObject& secondObject);
void asteroidsCollide(GameObject& firstObject, GameObject& secondObject);
void spawnAsteroid(int xPos, int yPos, Vec2 m_velocity, AsteroidSizeType sizeType, const std::list<GameObject>& gameobjects);

class Shot : public GameObject
{
public:
	Shot(float midPosX, float midPosY, Vec2 m_velocity, float shotHeadingAngle);
	void Update(int windowWidth, int windowHeight, float deltaTime);
	void Render(SDL_Renderer* renderer) override;

	static std::list<Shot> shots;
	static void SetTexture(SDL_Texture* texture) { s_texture = texture; }

private:
	inline static SDL_Texture* s_texture;

	Uint32 m_creationTime	= 0;
	Uint32 m_maxLifeTime	= 1000;
	bool tooOld();

	float m_rotation		= 0.f;
};

class Bomb : public GameObject
{
public:
	Bomb(int xPos, int yPos, Vec2 m_velocity);
	static void SetTexture(SDL_Texture* texture) { s_texture = texture; }

	Uint32 m_creationTime = 0;
	Uint32 m_ignitionTime = 0;
	void Update(int windowWidth, int windowHeight, float deltaTime, Ship* ship);
	void Render(SDL_Renderer* renderer) override;
	void GetCollected();
	void Explode();
	bool m_isDead = false;
	bool isExploding = false;
	bool isCollected = false;

private:
	float m_rotation		= 0.0f;
	float m_rotatingSpeed	= 10.f;
	inline static SDL_Texture* s_texture;
};

Vec2 calcPosIfLeaving(Vec2 m_midPos, float radius, int windowWidth, int windowHeight);