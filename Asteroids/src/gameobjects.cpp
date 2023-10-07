#include "gameobjects.hpp"

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

#define PI 3.14159265359f

SDL_Rect GameObject::getRenderRect() const
{
	SDL_Rect rect;
	rect.w = m_width;
	rect.h = m_height;
	rect.x = (int)std::round(m_midPos.x - m_width / 2);
	rect.y = (int)std::round(m_midPos.y - m_height / 2);
	return rect;
}

Ship::Ship(Vec2 midPos, int size, SDL_Texture* texture) 
	: GameObject(midPos, 0.f)
{
	m_width = size;
	m_height = size;
	m_colRadius = size / 2 * sizeToCollisonRadiusRatio;

	animationCounter = 0;
	timeLastShot = SDL_GetTicks();
	timeLastBomb = SDL_GetTicks();

	objectType = Type::Ship;
}

Ship::Ship() : GameObject()
{
}

void Ship::update(const InputHandler& MyInputHandler, int windowWidth, int windowHeight, float deltaTime)
{
	updateVisibility(deltaTime);
	updateTransform(MyInputHandler, windowWidth, windowHeight, deltaTime);
	updateAnimation(MyInputHandler, deltaTime);

	if ((MyInputHandler.getControlBools()).isShooting)
		shoot();

	if ((MyInputHandler.getControlBools()).isUsingBomb)
		useBomb();
}

void Ship::updateVisibility(float deltaTime)
{
	if (m_isVisible)
	{
		canBomb = true;
		shotCounter = std::max((shotCounter - shotCounterDecay * deltaTime), 0.0f);
		if (shotCounter >= maxShotCounter)
			canShoot = false;
		if (!canShoot && shotCounter <= shipCooldownThreshold)
			canShoot = true;
	}
	else
	{
		canBomb = false;
		canShoot = false;
		shotCounter = maxShotCounter + 1; // turns the UI red
		timeNotVisible += deltaTime;
		if (timeNotVisible > respawnTime)
		{
			timeNotVisible = 0;
			m_isVisible = true;
			shotCounter = 0;
		}
	}
}

void Ship::updateTransform(const InputHandler& MyInputHandler, int windowWidth, int windowHeight, float deltaTime)
{
	ControlBools CurrentControlBools = MyInputHandler.getControlBools();

	// Update transaltion
	float scalarVelocity = m_velocity.Length();
	float velocityAngle = atan2(m_velocity.x, m_velocity.y);

	scalarVelocity = std::max(scalarVelocity - velocityDecay * deltaTime, 0.0f);
	m_velocity.x = (sin(velocityAngle) * scalarVelocity);
	m_velocity.y = (cos(velocityAngle) * scalarVelocity);

	if (CurrentControlBools.giveThrust && scalarVelocity < velocityMax)
	{
		// TODO: refactor to Vec(1,1) with rotation
		float deltaVelocityX = sinf(rotation * PI / 180) * thrust * deltaTime;
		float deltaVelocityY = -(cosf(rotation * PI / 180)) * thrust * deltaTime;
		m_velocity.x += deltaVelocityX;
		m_velocity.y += deltaVelocityY;
	}

	m_midPos += m_velocity * deltaTime;

	// If ship leaves the screen, re-enter at opposite site
	m_midPos = calcPosIfLeaving(m_midPos, 0, windowWidth, windowHeight);

	// Update rotation
	if (CurrentControlBools.isTurningRight)
	{
		rotation += roatatingSpeed * deltaTime;
	}

	if (CurrentControlBools.isTurningLeft)
	{
		rotation -= roatatingSpeed * deltaTime;
	}
}

void Ship::updateAnimation(const InputHandler& MyInputHandler, float deltaTime)
{
	ControlBools CurrentControlBools = MyInputHandler.getControlBools();
	if (CurrentControlBools.giveThrust)
	{
		if (SDL_GetTicks() - timeLastUpdated > timeBetweenSprites)
		{
			animationCounter++;
			animationCounter = animationCounter % spriteCount + 1;
			timeLastUpdated = SDL_GetTicks();
		}
	}
	else
	{
		animationCounter = 0;
	}
}

void Ship::shoot()
{
	Uint32 timeSinceLastShot = SDL_GetTicks() - timeLastShot;

	if (canShoot && timeSinceLastShot > timeBetweenShots && shotCounter < maxShotCounter)
	{
		createShot();
		shotCounter = shotCounter + 100;

		timeLastShot = SDL_GetTicks();
	}
}

void Ship::createShot()
{
	Vec2 shotVelocityVector = { 0, 0 };

	shotVelocityVector.x = sin(rotation / 180 * PI) * shotVelocity + m_velocity.x;
	shotVelocityVector.y = -cos(rotation / 180 * PI) * shotVelocity + m_velocity.y;

	Shot(m_midPos.x, m_midPos.y, shotVelocityVector, rotation);
}

void Ship::useBomb()
{
	Uint32 timeSinceLastBomb = SDL_GetTicks() - timeLastBomb;

	if (!collectedBombs.empty() && canBomb && timeSinceLastBomb > timeBetweenBombs)
	{
		auto iterator = collectedBombs.begin();
		Bomb* Bomb = *iterator;

		assert(Bomb);
		Bomb->explode();

		collectedBombs.erase(iterator);

		timeLastBomb = SDL_GetTicks();
	}
}

void Ship::render(SDL_Renderer* renderer)
{
	assert(renderer && s_texture);

	renderShotMeter(renderer);
	renderShip(renderer);
}

void Ship::renderShotMeter(SDL_Renderer* renderer)
{
	SDL_Color meterColor = canShoot ? SDL_Color{ 0, 200, 0, 255 } : SDL_Color{ 200, 0, 0, 255 }; // Green : red

	/// Render meter triangle.
	/// Meter grows starting from the ships nose.
	Vec2 shipNose;
	float noseDistanceToShipMid = m_height * 0.42f;
	shipNose.x = m_midPos.x + SDL_sinf(rotation / 180 * PI) * noseDistanceToShipMid;
	shipNose.y = m_midPos.y - SDL_cosf(rotation / 180 * PI) * noseDistanceToShipMid;

	/// Render black base layer.
	SDL_Color triangleBaseColor = { 0, 0, 0, 255 };
	float tringleWidth = m_width * 0.5f;
	float trinagleHeight = m_height * 0.625f;
	drawTriangle(renderer, shipNose.x, shipNose.y, tringleWidth, trinagleHeight, rotation, triangleBaseColor);

	/// Overlay actual shot meter triangle
	float shotMeterValue = std::min(shotCounter / maxShotCounter, 1.0f); // from 0 to 1
	if (shotMeterValue > 0.1f)
	{
		drawTriangle(renderer, shipNose.x, shipNose.y, shotMeterValue * tringleWidth, shotMeterValue * trinagleHeight, rotation, meterColor);
	}
}

void Ship::renderShip(SDL_Renderer* renderer)
{
	// Grey out texture during respawn
	if (!m_isVisible)
	{
		float timeStepSize = 0.25f;
		int stepValue = floor(timeNotVisible / timeStepSize);
		if (stepValue % 2 == 0)
			SDL_SetTextureColorMod(s_texture, 100, 100, 100);
		else
			SDL_SetTextureColorMod(s_texture, 255, 255, 255);
	}
	else
	{
		SDL_SetTextureColorMod(s_texture, 255, 255, 255);
	}

	int currentSpriteStart = spriteWidth * animationCounter;
	SDL_Rect srcR{ currentSpriteStart,
				  0,
				  spriteWidth,
				  spriteHeight };
	SDL_Rect destR = getRenderRect();

	SDL_RenderCopyEx(renderer, s_texture, &srcR, &destR, rotation, NULL, SDL_FLIP_NONE);
}

void Ship::collectBomb(Bomb* bomb)
{
	collectedBombs.push_back(bomb);
	bomb->getCollect();
}

void Ship::reset(SDL_Renderer* renderer)
{
	int windowWidth, windowHeight;
	SDL_RenderGetLogicalSize(renderer, &windowWidth, &windowHeight);

	m_midPos = { windowWidth / 2.0f, windowHeight / 2.0f };
	m_velocity = { 0.0f, 0.0f };
	rotation = 0.f;
}

void Ship::respawn(SDL_Renderer* renderer)
{
	reset(renderer);
	m_isVisible = false;
}

std::list<Asteroid> Asteroid::asteroids;

Asteroid::Asteroid(Vec2 midPos, Vec2 velocity, AsteroidSizeType sizeType) 
	: GameObject(midPos, velocity), sizeType(sizeType)
{
	int size = getSize(sizeType);
	m_colRadius = getColRadius(size);
	m_width = size;
	m_height = size;

	if (sizeType == AsteroidSizeType::Small)
		objectType = Type::AsteroidSmall;
	else
		objectType = Type::AsteroidMedium;

	asteroids.push_back(*this);
}

int Asteroid::getSize(AsteroidSizeType sizeType)
{
	int size = -1;
	if (sizeType == AsteroidSizeType::Small)
		size = m_sizeSmall;
	else if (sizeType == AsteroidSizeType::Medium)
		size = m_sizeMedium;
	return size;
}

float Asteroid::getColRadius(int size)
{
	return size / 2.f * m_colRadiusFactor;
}

void Asteroid::update(int windowWidth, int windowHeight, float deltaTime)
{
	if (m_isVisible)
	{
		m_midPos.x += m_velocity.x * deltaTime * 60;
		m_midPos.y += m_velocity.y * deltaTime * 60;
	}

	Vec2 newMidPosistion = calcPosIfLeaving(m_midPos, m_colRadius, windowWidth, windowHeight);

	if ((m_midPos.x != newMidPosistion.x) || (m_midPos.y != newMidPosistion.y))
	{
		m_isVisible = false;
	}

	if (!m_isVisible)
	{
		m_isVisible = true;
		bool canStayVisible = true;
		for (const Asteroid& otherAsteroid : Asteroid::asteroids)
		{
			if (m_id == otherAsteroid.m_id)
				continue;
			if (doesCollide(*this, otherAsteroid))
			{
				canStayVisible = false;
				break;
			}
		}
		if (!canStayVisible)
		{
			m_isVisible = false;
		}
	}

	m_midPos = newMidPosistion;
}

void Asteroid::handleDestruction()
{
	int newAsteroidSize = Asteroid::getSize(AsteroidSizeType::Small);

	Vec2 spawnDirection = m_velocity.Rotate(90);
	spawnDirection.SetLength(newAsteroidSize / 2.f);

	Asteroid(
		m_midPos + spawnDirection,
		m_velocity.Rotate(45) * m_DestAstroidVelFactor,
		AsteroidSizeType::Small);

	Asteroid(
		m_midPos - spawnDirection,
		m_velocity.Rotate(-45) * m_DestAstroidVelFactor,
		AsteroidSizeType::Small);
}

void Asteroid::render(SDL_Renderer* renderer)
{
	SDL_Texture* asteroidTex = nullptr;
	if (sizeType == AsteroidSizeType::Medium)
	{
		asteroidTex = s_textureMedium;
	}
	else if (sizeType == AsteroidSizeType::Small)
	{
		asteroidTex = s_textureSmall;
	}
	else
	{
		throw std::runtime_error("Unknown AsteroidSizeType for rendering");
	}
	if (m_isVisible)
	{
		SDL_Rect asteroidRect = getRenderRect();
		SDL_RenderCopyEx(renderer, asteroidTex, NULL, &asteroidRect, 0.0f, NULL, SDL_FLIP_NONE);
	}
}

bool doesCollide(const GameObject& firstObject, const GameObject& secondObject)
{
	if (!firstObject.getVisibility() || !secondObject.getVisibility())
		return false;

	Vec2 firstMidPos = firstObject.getMidPos();
	float firstColRadius = firstObject.getColRadius();
	Vec2 secondMidPos = secondObject.getMidPos();
	float secondColRadius = secondObject.getColRadius();

	float squareDistance = (firstMidPos.x - secondMidPos.x) * (firstMidPos.x - secondMidPos.x) + (firstMidPos.y - secondMidPos.y) * (firstMidPos.y - secondMidPos.y);
	float squareColDistance = (firstColRadius + secondColRadius) * (firstColRadius + secondColRadius);

	return squareDistance <= squareColDistance;
}

struct CollisionOccurrence
{
	int firstObjectId;
	int secondObjectId;
	Uint32 time;
};

std::vector<CollisionOccurrence> recentCollisions = {};

bool didRecentlyCollide(GameObject firstObject, GameObject secondObject)
{
	int firstID = firstObject.getID();
	int secondID = secondObject.getID();

	for (auto it = recentCollisions.begin(); it != recentCollisions.end(); it++)
	{
		if ((it->firstObjectId == firstID && it->secondObjectId == secondID) || (it->firstObjectId == secondID && it->secondObjectId == firstID))
		{
			if (SDL_GetTicks() > (it->time + 500))
			{
				it->time = SDL_GetTicks();
				return false;
			}
			else
			{
				// std::cout << "Did recently collide" << std::endl;
				return true;
			}
		}
	}
	CollisionOccurrence collisionOccurrence;
	collisionOccurrence.firstObjectId = firstID;
	collisionOccurrence.secondObjectId = secondID;
	collisionOccurrence.time = SDL_GetTicks();
	recentCollisions.push_back(collisionOccurrence);
	return false;
}

void asteroidsCollide(GameObject& firstObject, GameObject& secondObject)
{
	if (doesCollide(firstObject, secondObject) && !didRecentlyCollide(firstObject, secondObject))
	{
		// source: https://docplayer.org/39258364-Ein-und-zweidimensionale-stoesse-mit-computersimulation.html
		Vec2 firstObjectMidPos = firstObject.getMidPos();
		Vec2 firstObjectVelocity = firstObject.getVelocity();
		Vec2 secondObjectMidPos = secondObject.getMidPos();
		Vec2 secondObjectVelocity = secondObject.getVelocity();

		// distance vector
		float normal[2];
		normal[0] = secondObjectMidPos.x - firstObjectMidPos.x;
		normal[1] = secondObjectMidPos.y - firstObjectMidPos.y;

		// angle between object 1 and normal
		float f1 = (firstObjectVelocity.x * normal[0] + firstObjectVelocity.y * normal[1]) / (normal[0] * normal[0] + normal[1] * normal[1]);
		// parallel component for object 1
		std::vector<float> vp1 = { normal[0] * f1, normal[1] * f1 };
		// vertical component for object 1
		std::vector<float> vv1 = { firstObjectVelocity.x - vp1[0], firstObjectVelocity.y - vp1[1] };

		// angle between object 2 and normal
		float f2 = (secondObjectVelocity.x * normal[0] + secondObjectVelocity.y * normal[1]) / (normal[0] * normal[0] + normal[1] * normal[1]);
		// parallel component for object 2
		std::vector<float> vp2 = { normal[0] * f2, normal[1] * f2 };
		// vertical component for object 2
		std::vector<float> vv2 = { secondObjectVelocity.x - vp2[0], secondObjectVelocity.y - vp2[1] };

		int weightObject1 = PI * firstObject.getColRadius() * firstObject.getColRadius();
		int weightObject2 = PI * secondObject.getColRadius() * secondObject.getColRadius();

		if (weightObject1 == weightObject2)
		{
			float velocityX = vv1[0] + vp2[0];
			float velocityY = vv1[1] + vp2[1];
			firstObject.setVelocity(velocityX, velocityY);

			velocityX = vv2[0] + vp1[0];
			velocityY = vv2[1] + vp1[1];
			secondObject.setVelocity(velocityX, velocityY);
		}
		else
		{
			float weightFactorX = 2 * ((weightObject1 * vp1[0] + weightObject2 * vp2[0]) / (weightObject1 + weightObject2));
			float weightFactorY = 2 * ((weightObject1 * vp1[1] + weightObject2 * vp2[1]) / (weightObject1 + weightObject2));

			vp1[0] = weightFactorX - vp1[0];
			vp1[1] = weightFactorY - vp1[1];
			vp2[0] = weightFactorX - vp2[0];
			vp2[1] = weightFactorY - vp2[1];

			float velocityX = vv1[0] + vp1[0];
			float velocityY = vv1[1] + vp1[1];
			firstObject.setVelocity(velocityX, velocityY);

			velocityX = vv2[0] + vp2[0];
			velocityY = vv2[1] + vp2[1];
			secondObject.setVelocity(velocityX, velocityY);
		}
	}
}

void spawnAsteroid(int xPos, int yPos, Vec2 velocity, AsteroidSizeType sizeType, const std::list<GameObject>& gameobjects)
{
	GameObject collisionObject = GameObject();
	collisionObject.setMidPos((float)xPos, (float)yPos);
	collisionObject.setColRadius(Asteroid::getColRadius(Asteroid::getSize(sizeType)));

	bool isSafeToSpawn = true;
	for (auto it = gameobjects.begin(); it != gameobjects.end(); it++)
	{
		if (doesCollide(collisionObject, *it))
		{
			isSafeToSpawn = false;
			break;
		}
	}
	if (isSafeToSpawn)
	{
		Asteroid(
			collisionObject.getMidPos(),
			velocity,
			sizeType);
	}
}

std::list<Shot> Shot::shots;

Shot::Shot(float midPosX, float midPosY, Vec2 m_velocity, float shotHeadingAngle)
{
	this->m_velocity = m_velocity;
	this->m_midPos = { midPosX, midPosY };

	creationTime = SDL_GetTicks();

	vAngle = shotHeadingAngle;

	float colRadiusOffset = 0.3;
	int size = 20;
	m_colRadius = size * colRadiusOffset;
	this->m_width = size;
	this->m_height = size;

	objectType = Type::Shot;

	shots.push_back(*this);
}

void Shot::update(int windowWidth, int windowHeight, float deltaTime)
{
	m_midPos.x += m_velocity.x * deltaTime;
	m_midPos.y += m_velocity.y * deltaTime;
	// midPos = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);

	if (TooOld())
	{
		m_isDead = true;
	}
}

void Shot::render(SDL_Renderer* renderer)
{
	SDL_Rect rect = getRenderRect();
	SDL_RenderCopyEx(renderer, s_texture, NULL, &rect, vAngle, NULL, SDL_FLIP_NONE);
}

bool Shot::TooOld()
{
	Uint32 deltaTime = SDL_GetTicks() - creationTime;
	return (maxLifeTime < deltaTime);
}

Vec2 calcPosIfLeaving(Vec2 m_midPos, float radius, int windowWidth, int windowHeight)
{
	Vec2 newMidPos = m_midPos;

	if (m_midPos.x < 0 - radius) // leave to left.
	{
		newMidPos.x = windowWidth + radius;
	}
	else if (m_midPos.x > windowWidth + radius) // leave to right.
	{
		newMidPos.x = 0 - radius;
	}

	if (m_midPos.y < 0 - radius) // leave to top.
	{
		newMidPos.y = windowHeight + radius;
	}
	else if (m_midPos.y > windowHeight + radius) // leave to bottom.
	{
		newMidPos.y = 0 - radius;
	}

	return newMidPos;
}

Bomb::Bomb(int xPos, int yPos, Vec2 m_velocity)
{
	m_midPos.x = xPos;
	m_midPos.y = yPos;
	this->m_velocity = m_velocity;
	creationTime = SDL_GetTicks();
	m_isVisible = true;

	int size = 50;
	m_width = size;
	m_height = size;

	isCollected = false;
	isExploding = false;

	float colRadiusOffset = 0.3;
	m_colRadius = size * colRadiusOffset;

	objectType = Type::Bomb;
}

void Bomb::update(int windowWidth, int windowHeight, float deltaTime, Ship* ship)
{
	if (!isCollected && !isExploding)
	{
		m_midPos.x += m_velocity.x * deltaTime * 60;
		m_midPos.y += m_velocity.y * deltaTime * 60;
		m_midPos = calcPosIfLeaving(m_midPos, m_colRadius, windowWidth, windowHeight);

		angle += 10 * deltaTime;
	}
	if (isCollected && !isExploding)
	{
		m_midPos = ship->getMidPos();
	}
	if (isExploding)
	{
		float explosionVelocity = 20.0f;
		float timeSinceIgnition = (SDL_GetTicks() - ignitionTime) / 1000.0f;
		m_colRadius += timeSinceIgnition * explosionVelocity * deltaTime * 60;
		if (timeSinceIgnition > 1.0f)
		{
			m_isDead = true;
		}
	}
}

void Bomb::render(SDL_Renderer* renderer)
{
	if (m_isVisible)
	{
		SDL_Rect rect = getRenderRect();
		SDL_RenderCopyEx(renderer, s_texture, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
	}
}

void Bomb::getCollect()
{
	isCollected = true;
	m_isVisible = false;
}

void Bomb::explode()
{
	isExploding = true;
	m_isVisible = true;
	ignitionTime = SDL_GetTicks();
}