#pragma once

#include <iostream>
#include <math.h>
#include <vector>
#include <list>
#include <stdexcept>

#include "inputhandler.hpp"
#include "shapes.hpp"

class GameObject
{
public:
	GameObject() : id(s_NewId++) {};
	static void resetId() { s_NewId = 1; };

	virtual void update() {};
	virtual void render(SDL_Renderer* renderer) {};

	enum class Type {
		Default,
		Ship,
		AsteroidSmall,
		AsteroidMedium,
		Bomb,
		Shot
	};

	Type objectType = Type::Default;

protected:

	int id = -1;
	int width = 0;
	int height = 0;
	float colRadius = 0;
	bool isVisible = true; // Indicator, false during respawn.
	bool isDead = false;  // Can be destroyed.

	SDL_FPoint velocity = { 0, 0 };
	SDL_FPoint midPos = { 0, 0 };

	SDL_Rect getRenderRect() const;

private:
	inline static int s_NewId = 0;

public:
	int getID() const { return id; };
	int getWidth() const { return width; };
	int getHeight() const { return height; };
	float getColRadius() const { return colRadius; };
	bool getVisibility() const { return isVisible; };
	bool getIsDead() const { return isDead; };
	SDL_FPoint getMidPos() const { return midPos; };
	SDL_FPoint getVelocity() const { return velocity; };

	void setColRadius(float radius) { colRadius = radius; };
	void setMidPos(float x, float y) { midPos = { x, y }; };
	void setVelocity(float x, float y) { velocity = { x, y }; };
};

class Ship : public GameObject
{
public:
	Ship();
	Ship(int midPosX, int midPosY, int size, SDL_Texture* texture);

	void update(const InputHandler& MyInputHandler, int windowWidth, int windowHeight, float deltaTime);
	void render(SDL_Renderer* renderer) override;
	void reset(SDL_Renderer* renderer);
	void respawn(SDL_Renderer* renderer);

	static void setTexture(SDL_Texture* texture) { s_texture = texture; }

private:
	inline static SDL_Texture* s_texture;
	// Update shooting capability and ship visibility.
	// Ship is not visible during respawn.
	float respawnTime = 3;
	float timeNotVisible = 0;
	void updateVisibility(float deltaTime);

	// movement values
	float velocityMax = 1000;
	float velocityDecay = 50;
	float rotation = 0; // Current rotation in degree
	float roatatingSpeed = 180.0;
	float thrust = 350.0;
	void updateTransform(const InputHandler& MyInputHandler, int windowWidth, int windowHeight, float deltaTime);
	void updateAnimation(const InputHandler& MyInputHandler, float deltaTime);

	// Shooting values
	float shotVelocity = 1000.f;
	float shotCounter = 0.0f;
	float shotCounterDecay = 150.0f;
	float maxShotCounter = 1000.0f;
	float shipCooldownThreshold = maxShotCounter / 2.f;

	bool canShoot = true; // will set to false during respawn
	Uint32 timeLastShot = 0;
	Uint32 timeBetweenShots = 250;
	void shoot();
	void createShot();

	// Bombing values
	std::list<class Bomb*> collectedBombs;

	bool canBomb = true; // will set to false during respawn
	Uint32 timeLastBomb = 0;
	Uint32 timeBetweenBombs = 250;
	void useBomb();

	// Animation values
	int spriteWidth = 300;
	int spriteHeight = 300;
	int spriteCount = 3;  // Number of different sprites in texture
	int animationCounter = 0; // Used to select sprite
	unsigned timeBetweenSprites = 300;
	Uint32 timeLastUpdated = 0;
	void renderShotMeter(SDL_Renderer* renderer);
	void renderShip(SDL_Renderer* renderer);

	// collision values
	float sizeToCollisonRadiusRatio = 0.6f;

public:
	float getMaxVelocity() { return velocityMax; };
	float getShotCounter() { return shotCounter; };
	float getMaxShotCounter() { return maxShotCounter; };
	bool getCanShoot() { return canShoot; };
	float getShotVelocity() { return shotVelocity; };
	void collectBomb(class Bomb* bomb);
	int getCollectedBombsSize() { return (int)collectedBombs.size(); };
};

enum class AsteroidSizeType
{
	Small,
	Medium
};

class Asteroid : public GameObject
{
public:
	Asteroid(float midPosX, float midPosY, SDL_FPoint velocity, AsteroidSizeType sizeType);
	void update(int windowWidth, int windowHeight, float deltaTime);
	void render(SDL_Renderer* renderer) override;

	AsteroidSizeType sizeType;

	static std::list<Asteroid> asteroids;
	static float getColRadius(int size);
	static int getSize(AsteroidSizeType sizeType);

	static void setTextureSmall(SDL_Texture* texture) { s_textureSmall = texture; }
	static void setTextureMedium(SDL_Texture* texture) { s_textureMedium = texture; }

private:
	inline static SDL_Texture* s_textureSmall;
	inline static SDL_Texture* s_textureMedium;
};

// void initShip(int windowWidth, int windowHeight);
bool doesCollide(const GameObject& firstObject, const GameObject& secondObject);
void asteroidsCollide(GameObject& firstObject, GameObject& secondObject);
void handleDestruction(Asteroid destoryedAsteroid);
void spawnAsteroid(int xPos, int yPos, SDL_FPoint velocity, AsteroidSizeType sizeType, const std::list<GameObject>& gameobjects);

class Shot : public GameObject
{
public:
	Shot(float midPosX, float midPosY, SDL_FPoint velocity, float shotHeadingAngle);
	void update(int windowWidth, int windowHeight, float deltaTime);
	void render(SDL_Renderer* renderer) override;

	static std::list<Shot> shots;
	static void setTexture(SDL_Texture* texture) { s_texture = texture; }

private:
	inline static SDL_Texture* s_texture;

	Uint32 creationTime;
	Uint32 maxLifeTime = 1000;
	bool TooOld();

	float vAngle = 0;
};

class Bomb : public GameObject
{
public:
	Bomb(int xPos, int yPos, SDL_FPoint velocity);
	static void setTexture(SDL_Texture* texture) { s_texture = texture; }

private:
	float angle = 0.0f;
	inline static SDL_Texture* s_texture;

public:
	Uint32 creationTime = 0;
	Uint32 ignitionTime = 0;
	void update(int windowWidth, int windowHeight, float deltaTime, Ship* ship);
	void render(SDL_Renderer* renderer) override;
	void getCollect();
	void explode();
	bool isDead = false;
	bool isExploding = false;
	bool isCollected = false;
};

SDL_FPoint calcPosIfLeaving(SDL_FPoint midPos, float radius, int windowWidth, int windowHeight);