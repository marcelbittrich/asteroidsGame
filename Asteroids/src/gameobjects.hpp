#pragma once

#include <iostream>
#include <math.h>
#include <vector>
#include <list>
#include <stdexcept>

#include "inputhandler.hpp"
#include "shapes.hpp"
#include "vector2.hpp"
#include "audioplayer.hpp"

struct GameObjectShared
{
	SDL_Renderer* renderer;
	AudioPlayer* audioPlayer;

};

class GameObject
{
public:
	GameObject() : m_id(s_NewId++) {}
	GameObject(Vec2 midPos, Vec2 velocity) 
		: m_id(s_NewId++), m_midPos(midPos), m_velocity(velocity) {}

	static void ResetId() { s_NewId = 1; }
	virtual void Update() {};
	virtual void Render() {};

	enum class Type {
		Default,
		Ship,
		Asteroid,
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
	void SetIsDead(bool value) { m_isDead = value; };

	static void SetRenderer(SDL_Renderer* renderer) { s_renderer = renderer; };
	static void SetAudioPlayer(AudioPlayer* audioPlayer) { s_audioPlayer = audioPlayer; };

protected:
	int m_id			= -1;
	int m_width			= 0;
	int m_height		= 0;
	float m_colRadius	= 0.f;
	bool m_isVisible	= true;		// Indicator, false during respawn.
	bool m_isDead		= false;	// Can be destroyed.

	Vec2 m_midPos;
	Vec2 m_velocity;

	SDL_FRect GetRenderRect() const;
	inline static SDL_Renderer* s_renderer = nullptr;
	inline static AudioPlayer* s_audioPlayer = nullptr;

private:
	inline static int s_NewId = 0;
};

class Ship : public GameObject
{
public:
	Ship() {};
	Ship(Vec2 midPos, int size, SDL_Texture* texture);

	void Update(const InputHandler& myInputHandler, int windowWidth, int windowHeight, float deltaTime);
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

	static void SetTexture(SDL_Texture* texture) { s_texture = texture; }
	inline static std::vector<Ship> ships;

private:
	inline static SDL_Texture* s_texture;
	// Update shooting capability and ship visibility.
	// Ship is not visible during respawn.
	float m_respawnTime			= 3.f;
	float m_timeNotVisible		= 0.f;
	void UpdateVisibility(float deltaTime);

	// movement values
	float m_velocityMax			= 1000.f;
	float m_velocityDecay		= 50.f;
	float m_rotation			= 0.f;
	float m_roatatingSpeed		= 180.f;
	float m_thrust				= 350.f;
	void UpdateTransform(const InputHandler& myInputHandler, int windowWidth, int windowHeight, float deltaTime);
	void UpdateAnimation(const InputHandler& myInputHandler, float deltaTime);

	// Shooting values
	float m_shotVelocity		= 1000.f;
	float m_shotCounter			= 0.0f;
	float m_shotCounterDecay	= 150.0f;
	float m_maxShotCounter		= 1000.0f;
	float m_shipCooldownThreshold = m_maxShotCounter / 2.f;

	bool m_canShoot				= true;	// Indicator, false during respawn.
	Uint32 m_timeLastShot		= 0;
	Uint32 m_timeBetweenShots	= 250;
	void Shoot();
	void CreateShot();

	// Bombing values
	std::list<class Bomb*> m_collectedBombs;

	bool m_canBomb				= true;	// Indicator, false during respawn.
	Uint32 m_timeLastBomb		= 0;
	Uint32 m_timeBetweenBombs	= 250;
	void UseBomb();

	// Animation values
	int m_spriteWidth			= 300;
	int m_spriteHeight			= 300;
	int m_spriteCount			= 3;	// Number of different sprites in texture
	int m_animationCounter		= 0;	// Used to select sprite
	Uint32 m_timeBetweenSprites = 300;
	Uint32 m_timeLastUpdated	= 0;
	void RenderShotMeter();
	void RenderShip();

	// collision values
	float m_colRadiusFactor		= 0.6f;
};

class Asteroid : public GameObject
{
public:
	enum class SizeType
	{
		Small,
		Medium
	};

	Asteroid(Vec2 m_midPos, Vec2 m_velocity, Asteroid::SizeType sizeType);
	void Update(int windowWidth, int windowHeight, float deltaTime);
	void Render() override;

	SizeType sizeType;

	inline static std::list<Asteroid> asteroids;
	static int GetSize(SizeType sizeType);
	static float GetColRadius(int size);

	static void SetTextureSmall(SDL_Texture* texture) { s_textureSmall = texture; }
	static void SetTextureMedium(SDL_Texture* texture) { s_textureMedium = texture; }

	void HandleDestruction();

private:
	inline static const float m_colRadiusFactor = 0.6f;
	const float m_DestAstroidVelFactor			= 2.0;
	static const int m_sizeSmall				= 50;
	static const int m_sizeMedium				= 100;
	inline static SDL_Texture* s_textureSmall;
	inline static SDL_Texture* s_textureMedium;
};

void spawnAsteroid(int xPos, int yPos, Vec2 m_velocity, Asteroid::SizeType sizeType, const std::list<GameObject*>& gameobjects);

class Shot : public GameObject
{
public:
	Shot(Vec2 midPos, Vec2 velocity, float shotHeadingAngle);
	void Update(int windowWidth, int windowHeight, float deltaTime);
	void Render() override;

	inline static std::list<Shot> shots;
	static void SetTexture(SDL_Texture* texture) { s_texture = texture; }

private:
	inline static SDL_Texture* s_texture;

	int m_size				= 20;
	float m_colRadiusFactor = 0.3f;

	Uint32 m_creationTime	= 0;
	Uint32 m_maxLifeTime	= 1000;
	bool TooOld();

	float m_rotation		= 0.f;
};

class Bomb : public GameObject
{
public:
	Bomb(Vec2 midPos, Vec2 velocity);
	void Update(int windowWidth, int windowHeight, float deltaTime);
	void Render() override;
	void GetCollected(Ship* ownerShip);
	void Explode();

	inline static std::list<Bomb> bombs;
	static void SetTexture(SDL_Texture* texture) { s_texture = texture; }

	bool GetIsExploding() const { return isExploding; };

private:
	int m_size = 50;
	float m_colRadiusFactor = 0.3f;

	float m_explodindVelocity = 600.f;
	float m_maxExplodingRadius = 600.f;

	bool isExploding = false;
	bool isCollected = false;

	//Uint32 m_creationTime = 0;
	//Uint32 m_ignitionTime = 0;

	float m_rotation		= 0.0f;
	float m_rotatingSpeed	= 10.f;
	inline static SDL_Texture* s_texture;

	Ship* m_ownerShip = { nullptr };
};

Vec2 calcPosIfLeaving(Vec2 m_midPos, float radius, int windowWidth, int windowHeight);