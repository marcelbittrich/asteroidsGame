#include "gameobjects.hpp"

// uncomment to disable assert()
// #define NDEBUG
#include <cassert>

#define PI 3.14159265359f

//int GameObject::s_NewId = 0;

SDL_Rect GameObject::getRenderRect() const
{
	SDL_Rect rect;
	rect.w = width;
	rect.h = height;
	rect.x = (int)std::round(midPos.x - width / 2);
	rect.y = (int)std::round(midPos.y - height / 2);
	return rect;
}

Ship::Ship(int midPosX, int midPosY, int size, SDL_Texture* texture) 
	: GameObject() 
{
	width = size;
	height = size;

	colRadius = size / 2 * sizeToCollisonRadiusRatio;
	midPos = { (float)midPosX, (float)midPosY };

	animationCounter = 0;
	timeLastShot = SDL_GetTicks();
	timeLastBomb = SDL_GetTicks();

	m_texture = texture;

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
	if (isVisible)
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
			isVisible = true;
			shotCounter = 0;
		}
	}
}

void Ship::updateTransform(const InputHandler& MyInputHandler, int windowWidth, int windowHeight, float deltaTime)
{
	ControlBools CurrentControlBools = MyInputHandler.getControlBools();

	// Update transaltion
	float scalarVelocity = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
	float velocityAngle = atan2(velocity.x, velocity.y);

	scalarVelocity = std::max(scalarVelocity - velocityDecay * deltaTime, 0.0f);
	velocity.x = (sin(velocityAngle) * scalarVelocity);
	velocity.y = (cos(velocityAngle) * scalarVelocity);

	if (CurrentControlBools.giveThrust && scalarVelocity < velocityMax)
	{
		float deltaVelocityX = sinf(rotation * PI / 180) * thrust * deltaTime;
		float deltaVelocityY = -(cosf(rotation * PI / 180)) * thrust * deltaTime;
		velocity.x += deltaVelocityX;
		velocity.y += deltaVelocityY;
	}

	midPos.x += velocity.x * deltaTime;
	midPos.y += velocity.y * deltaTime;

	// If ship leaves the screen, re-enter at opposite site
	midPos = calcPosIfLeaving(midPos, 0, windowWidth, windowHeight);

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
	SDL_FPoint shotVelocityVector = { 0, 0 };

	shotVelocityVector.x = sin(rotation / 180 * PI) * shotVelocity + velocity.x;
	shotVelocityVector.y = -cos(rotation / 180 * PI) * shotVelocity + velocity.y;

	Shot(midPos.x, midPos.y, shotVelocityVector, rotation);
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
	assert(renderer && m_texture);

	renderShotMeter(renderer);
	renderShip(renderer);
}

void Ship::renderShotMeter(SDL_Renderer* renderer)
{
	SDL_Color meterColor = canShoot ? SDL_Color{ 0, 200, 0, 255 } : SDL_Color{ 200, 0, 0, 255 }; // Green : red

	/// Render meter triangle.
	/// Meter grows starting from the ships nose.
	SDL_FPoint shipNose;
	float noseDistanceToShipMid = height * 0.42f;
	shipNose.x = midPos.x + SDL_sinf(rotation / 180 * PI) * noseDistanceToShipMid;
	shipNose.y = midPos.y - SDL_cosf(rotation / 180 * PI) * noseDistanceToShipMid;

	/// Render black base layer.
	SDL_Color triangleBaseColor = { 0, 0, 0, 255 };
	float tringleWidth = width * 0.5f;
	float trinagleHeight = height * 0.625f;
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
	if (!isVisible)
	{
		float timeStepSize = 0.25f;
		int stepValue = floor(timeNotVisible / timeStepSize);
		if (stepValue % 2 == 0)
			SDL_SetTextureColorMod(m_texture, 100, 100, 100);
		else
			SDL_SetTextureColorMod(m_texture, 255, 255, 255);
	}
	else
	{
		SDL_SetTextureColorMod(m_texture, 255, 255, 255);
	}

	int currentSpriteStart = spriteWidth * animationCounter;
	SDL_Rect srcR{ currentSpriteStart,
				  0,
				  spriteWidth,
				  spriteHeight };
	SDL_Rect destR = getRenderRect();

	SDL_RenderCopyEx(renderer, m_texture, &srcR, &destR, rotation, NULL, SDL_FLIP_NONE);
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

	midPos = { windowWidth / 2.0f, windowHeight / 2.0f };
	velocity = { 0.0f, 0.0f };
	rotation = 0.f;
}

void Ship::respawn(SDL_Renderer* renderer)
{
	reset(renderer);
	isVisible = false;
}

std::list<Asteroid> Asteroid::asteroids;

Asteroid::Asteroid(float midPosX, float midPosY, SDL_FPoint velocity, AsteroidSizeType sizeType) : GameObject()
{
	this->midPos = { midPosX, midPosY };
	this->sizeType = sizeType;
	this->velocity = velocity;
	int size = getSize(sizeType);
	this->colRadius = getColRadius(size);
	this->width = size;
	this->height = size;

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
		size = 50;
	if (sizeType == AsteroidSizeType::Medium)
		size = 100;
	return size;
}

float Asteroid::getColRadius(int size)
{
	float colRadiusOffset = 0.6;
	return (float)size / 2 * colRadiusOffset;
}

void Asteroid::update(int windowWidth, int windowHeight, float deltaTime)
{
	if (isVisible)
	{
		midPos.x += velocity.x * deltaTime * 60;
		midPos.y += velocity.y * deltaTime * 60;
	}

	SDL_FPoint newMidPosistion = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);

	if ((midPos.x != newMidPosistion.x) || (midPos.y != newMidPosistion.y))
	{
		isVisible = false;
	}

	if (!isVisible)
	{
		isVisible = true;
		bool canStayVisible = true;
		for (const Asteroid& otherAsteroid : Asteroid::asteroids)
		{
			if (id == otherAsteroid.id)
				continue;
			if (doesCollide(*this, otherAsteroid))
			{
				canStayVisible = false;
				break;
			}
		}
		if (!canStayVisible)
		{
			isVisible = false;
		}
	}

	midPos = newMidPosistion;
}

void Asteroid::render(SDL_Renderer* renderer, SDL_Texture* asteroidTexSmall, SDL_Texture* asteroidTexMedium)
{
	SDL_Texture* asteroidTex = nullptr;
	if (sizeType == AsteroidSizeType::Medium)
	{
		asteroidTex = asteroidTexMedium;
	}
	else if (sizeType == AsteroidSizeType::Small)
	{
		asteroidTex = asteroidTexSmall;
	}
	else
	{
		throw std::runtime_error("Unknown AsteroidSizeType for rendering");
	}
	if (isVisible)
	{
		SDL_Rect asteroidRect = getRenderRect();
		SDL_RenderCopyEx(renderer, asteroidTex, NULL, &asteroidRect, 0.0f, NULL, SDL_FLIP_NONE);
	}
}

bool doesCollide(const GameObject& firstObject, const GameObject& secondObject)
{
	if (!firstObject.getVisibility() || !secondObject.getVisibility())
		return false;

	SDL_FPoint firstMidPos = firstObject.getMidPos();
	float firstColRadius = firstObject.getColRadius();
	SDL_FPoint secondMidPos = secondObject.getMidPos();
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
		SDL_FPoint firstObjectMidPos = firstObject.getMidPos();
		SDL_FPoint firstObjectVelocity = firstObject.getVelocity();
		SDL_FPoint secondObjectMidPos = secondObject.getMidPos();
		SDL_FPoint secondObjectVelocity = secondObject.getVelocity();

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

void spawnAsteroid(int xPos, int yPos, SDL_FPoint velocity, AsteroidSizeType sizeType, const std::list<GameObject>& gameobjects)
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
			collisionObject.getMidPos().x,
			collisionObject.getMidPos().y,
			velocity,
			sizeType);
	}
}

std::list<Shot> Shot::shots;

Shot::Shot(float midPosX, float midPosY, SDL_FPoint velocity, float shotHeadingAngle)
{
	this->velocity = velocity;
	this->midPos = { midPosX, midPosY };

	creationTime = SDL_GetTicks();

	vAngle = shotHeadingAngle;

	float colRadiusOffset = 0.3;
	int size = 20;
	colRadius = size * colRadiusOffset;
	this->width = size;
	this->height = size;

	objectType = Type::Shot;

	shots.push_back(*this);
}

void Shot::update(int windowWidth, int windowHeight, float deltaTime)
{
	midPos.x += velocity.x * deltaTime;
	midPos.y += velocity.y * deltaTime;
	// midPos = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);

	if (TooOld())
	{
		isDead = true;
	}
}

void Shot::render(SDL_Renderer* renderer, SDL_Texture* shotTex)
{
	SDL_Rect rect = getRenderRect();
	SDL_RenderCopyEx(renderer, shotTex, NULL, &rect, vAngle, NULL, SDL_FLIP_NONE);
}

bool Shot::TooOld()
{
	Uint32 deltaTime = SDL_GetTicks() - creationTime;
	return (maxLifeTime < deltaTime);
}

SDL_FPoint calcPosIfLeaving(SDL_FPoint midPos, float radius, int windowWidth, int windowHeight)
{
	SDL_FPoint newMidPos = midPos;

	if (midPos.x < 0 - radius) // leave to left.
	{
		newMidPos.x = windowWidth + radius;
	}
	else if (midPos.x > windowWidth + radius) // leave to right.
	{
		newMidPos.x = 0 - radius;
	}

	if (midPos.y < 0 - radius) // leave to top.
	{
		newMidPos.y = windowHeight + radius;
	}
	else if (midPos.y > windowHeight + radius) // leave to bottom.
	{
		newMidPos.y = 0 - radius;
	}
	return newMidPos;
}

SDL_FPoint normalize2DVector(SDL_FPoint vector2D)
{
	float factor = 1 / SDL_sqrtf(pow(vector2D.x, 2) + pow(vector2D.y, 2));
	return { vector2D.x * factor, vector2D.y * factor };
}

SDL_FPoint set2DVectorLength(SDL_FPoint vector2D, float length)
{
	SDL_FPoint normalizedVector = normalize2DVector(vector2D);
	return { normalizedVector.x * length, normalizedVector.y * length };
}

SDL_FPoint rotate2DVector(SDL_FPoint old2DVector, float angleInDegree)
{
	SDL_FPoint rotated2DVector = { 0, 0 };

	float angleInRadian = angleInDegree * PI / 180;
	rotated2DVector.x = old2DVector.x * cosf(angleInRadian) - old2DVector.x * sinf(angleInRadian);
	rotated2DVector.y = old2DVector.y * sinf(angleInRadian) + old2DVector.y * cosf(angleInRadian);

	return rotated2DVector;
}

void handleDestruction(Asteroid destroyedAsteroid)
{
	int newAsteroidSize = Asteroid::getSize(AsteroidSizeType::Small);

	SDL_FPoint oldMidPos = destroyedAsteroid.getMidPos();
	SDL_FPoint oldVelocity = destroyedAsteroid.getVelocity();

	SDL_FPoint spawnDirection = rotate2DVector(oldVelocity, 90);
	spawnDirection = set2DVectorLength(spawnDirection, newAsteroidSize / 2);

	Asteroid(
		spawnDirection.x + oldMidPos.x,
		spawnDirection.y + oldMidPos.y,
		{ rotate2DVector(oldVelocity, 45).x * 2, rotate2DVector(oldVelocity, 45).y * 2 },
		AsteroidSizeType::Small);

	spawnDirection = rotate2DVector(spawnDirection, 180);
	spawnDirection = set2DVectorLength(spawnDirection, newAsteroidSize / 2);
	Asteroid(
		spawnDirection.x + oldMidPos.x,
		spawnDirection.y + oldMidPos.y,
		{ rotate2DVector(oldVelocity, -45).x * 2, rotate2DVector(oldVelocity, -45).y * 2 },
		AsteroidSizeType::Small);
}

Bomb::Bomb(int xPos, int yPos, SDL_FPoint velocity)
{
	midPos.x = xPos;
	midPos.y = yPos;
	this->velocity = velocity;
	creationTime = SDL_GetTicks();
	isVisible = true;

	int size = 50;
	width = size;
	height = size;

	isCollected = false;
	isExploding = false;

	float colRadiusOffset = 0.3;
	colRadius = size * colRadiusOffset;

	objectType = Type::Bomb;
}

void Bomb::update(int windowWidth, int windowHeight, float deltaTime, Ship* ship)
{
	if (!isCollected && !isExploding)
	{
		midPos.x += velocity.x * deltaTime * 60;
		midPos.y += velocity.y * deltaTime * 60;
		midPos = calcPosIfLeaving(midPos, colRadius, windowWidth, windowHeight);

		angle += 10 * deltaTime;
	}
	if (isCollected && !isExploding)
	{
		midPos = ship->getMidPos();
	}
	if (isExploding)
	{
		float explosionVelocity = 20.0f;
		float timeSinceIgnition = (SDL_GetTicks() - ignitionTime) / 1000.0f;
		colRadius += timeSinceIgnition * explosionVelocity * deltaTime * 60;
		if (timeSinceIgnition > 1.0f)
		{
			isDead = true;
		}
	}
}

void Bomb::render(SDL_Renderer* renderer, SDL_Texture* bombTex)
{
	if (isVisible)
	{
		SDL_Rect rect = getRenderRect();
		SDL_RenderCopyEx(renderer, bombTex, NULL, &rect, angle, NULL, SDL_FLIP_NONE);
	}
}

void Bomb::getCollect()
{
	isCollected = true;
	isVisible = false;
}

void Bomb::explode()
{
	isExploding = true;
	isVisible = true;
	ignitionTime = SDL_GetTicks();
}