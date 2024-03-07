#include "collisionhandler.h"

#include <iostream>
#include "math.h"

#include "../vector2.hpp"
#include "../game.hpp"
#include "../objects/initialization.hpp"
#include "../objects/gameobjects/asteroid.h"
#include "../objects/gameobjects/bomb.h"
#include "../objects/gameobjects/ship.h"
#include "../objects/gameobjects/shot.h"

void CollisionHandler::CheckCollisions(const std::vector<GameObject*>& gameObjectPtrs)
{
	// Offset to not check the same pairings again or object with itself
	int offset = 1;
	for (auto it1 = gameObjectPtrs.begin(); it1 != gameObjectPtrs.end(); it1++, offset++)
	{
		for (auto it2 = std::next(gameObjectPtrs.begin(), offset); it2 != gameObjectPtrs.end(); it2++)
		{
			GameObject* object1 = *it1;
			GameObject* object2 = *it2;

			if (DoesCollide(*object1, *object2) && !RecentlyCollide(*object1, *object2))
			{
 				const Type type1 = object1->objectType;
				const Type type2 = object2->objectType;

				if (ShipAndAnsteroid(type1, type2))
				{
					HandleShipAsteroidCollision(object1, object2);
				}
				else if (ShipAndBomb(type1, type2))
				{
					HandleShipBombCollision(object1, object2);
				}
				else if (ShipAndFollower(type1, type2))
				{
					HandleShipFollowerCollision(object1, object2);
				}
				else if (AsteroidAndAsteroid(type1, type2))
				{
					HandleAsteroidAsteroidCollision(object1, object2);
				}
				else if (AsteroidAndShot(type1, type2))
				{
					HandleAsteroidShotCollision(object1, object2);
				}
				else if (BombAndAsteroid(type1, type2))
				{
					HandleBombAsteroidCollision(object1, object2);
				}
				else if (FollowerAndShot(type1, type2))
				{
					HandleFollowerShotCollision(object1, object2);
				}
			}
		}
	}
}

void CollisionHandler::HandleShipAsteroidCollision(GameObject* object1, GameObject* object2)
{
	Ship* shipPtr = (object1->objectType == Type::Ship) ? (Ship*)object1 : (Ship*)object2;

	shipPtr->Respawn();
	Game::DecreaseLife();
}

void CollisionHandler::HandleShipBombCollision(GameObject* object1, GameObject* object2)
{
	Ship* shipPtr = (object1->objectType == Type::Ship) ? (Ship*)object1 : (Ship*)object2;
	Bomb* bombPtr = (object1->objectType == Type::Bomb) ? (Bomb*)object1 : (Bomb*)object2;

	if (!bombPtr->GetIsExploding())
	{
		shipPtr->CollectBomb(bombPtr);
	}
}

void CollisionHandler::HandleShipFollowerCollision(GameObject* object1, GameObject* object2)
{
	Ship* shipPtr = (object1->objectType == Type::Ship) ? (Ship*)object1 : (Ship*)object2;

	shipPtr->Respawn();
	Game::DecreaseLife();
}

void CollisionHandler::HandleAsteroidAsteroidCollision(GameObject* object1, GameObject* object2)
{
	// Elastic collision
	// source: https://docplayer.org/39258364-Ein-und-zweidimensionale-stoesse-mit-computersimulation.html
	Vec2 firstObjectMidPos		= object1->GetMidPos();
	Vec2 firstObjectVelocity	= object1->GetVelocity();
	Vec2 secondObjectMidPos		= object2->GetMidPos();
	Vec2 secondObjectVelocity	= object2->GetVelocity();

	// Distance vector
	Vec2 distance = secondObjectMidPos - firstObjectMidPos;
	float squareLength = distance.SquareLength();

	// Angle between object 1 and normal
	float f1 = Vec2::Dot(firstObjectVelocity, distance) / squareLength;

	// Parallel component for object 1
	Vec2 vp1 = distance * f1;

	// Vertical component for object 1
	Vec2 vv1 = firstObjectVelocity - vp1;

	// Angle between object 2 and normal
	float f2 = Vec2::Dot(secondObjectVelocity, distance) / squareLength;

	// Parallel component for object 2
	Vec2 vp2 = distance * f2;

	// Vertical component for object 2
	Vec2 vv2 = secondObjectVelocity - vp2;

	// Estimate weight by the surface area
	float weightObject1 = (float)M_PI * object1->GetColRadius() * object1->GetColRadius();
	float weightObject2 = (float)M_PI * object2->GetColRadius() * object2->GetColRadius();

	if (weightObject1 == weightObject2)
	{
		Vec2 velocity1 = vv1 + vp2;
		object1->SetVelocity(velocity1);

		Vec2 velocity2 = vv2 + vp1;
		object2->SetVelocity(velocity2);
	}
	else
	{
		Vec2 weightFactor = (vp1 * weightObject1 + vp2 * weightObject2) * 2.f / (weightObject1 + weightObject2);

		vp1 = weightFactor - vp1;
		vp2 = weightFactor - vp2;

		Vec2 velocity1 = vv1 + vp1;
		object1->SetVelocity(velocity1);

		Vec2 velocity2 = vv2 + vp2;
		object2->SetVelocity(velocity2);
	}
}

void CollisionHandler::HandleAsteroidShotCollision(GameObject* object1, GameObject* object2)
{
	Shot*		shotPtr     = (object1->objectType == Type::Shot) ? (Shot*)object1 : (Shot*)object2;
	Asteroid*	asteroidPtr = (object1->objectType == Type::Shot) ? (Asteroid*)object2 : (Asteroid*)object1;
	 
	if (asteroidPtr->sizeType == Asteroid::SizeType::Medium)
	{
		asteroidPtr->HandleDestruction();
	}
	else
	{
		m_game->GetAudioPlayer().PlaySoundEffect(EffectType::SmallAsteroidExplode);
		if (m_game->GetScore() % BOMB_SPAWN_ON_SCORE == 0)
		{
			Bomb newBomb = Bomb(asteroidPtr->GetMidPos(), GetRandomVelocity(0.0f, 0.5f));
			Game::bombs.push_back(newBomb);
		}
		m_game->IncreaseScore();
	}
	
	asteroidPtr->SetIsDead(true);
	shotPtr->SetIsDead(true);
}

void CollisionHandler::HandleBombAsteroidCollision(GameObject* object1, GameObject* object2)
{
	Bomb*		bombPtr		= (object1->objectType == Type::Bomb) ? (Bomb*)object1 : (Bomb*)object2;
	Asteroid*	asteroidPtr	= (object1->objectType == Type::Bomb) ? (Asteroid*)object2 : (Asteroid*)object1;

	if (bombPtr->GetIsExploding())
	{
		asteroidPtr->SetIsDead(true);
	}
}

void CollisionHandler::HandleFollowerShotCollision(GameObject* object1, GameObject* object2)
{
	Shot*     shotPtr     = (object1->objectType == Type::Shot) ? (Shot*)object1 : (Shot*)object2;
	Follower* followerPtr = (object1->objectType == Type::Shot) ? (Follower*)object2 : (Follower*)object1;

	Game::IncreaseScore();

	shotPtr->SetIsDead(true);
	followerPtr->SetIsDead(true);
}

bool CollisionHandler::DoesCollide(const GameObject& object1, const GameObject& object2)
{
	if (!object1.GetVisibility() || !object2.GetVisibility())
		return false;

	Vec2 firstMidPos = object1.GetMidPos();
	Vec2 secondMidPos = object2.GetMidPos();
	float squareDistance = Vec2::SquareDistance(firstMidPos, secondMidPos);

	float firstColRadius = object1.GetColRadius();
	float secondColRadius = object2.GetColRadius();
	float squareColDistance = (firstColRadius + secondColRadius) * (firstColRadius + secondColRadius);

	return squareDistance <= squareColDistance;
}

bool CollisionHandler::ShipAndAnsteroid(const Type& type1, const Type& type2)
{
	return (type1 == Type::Ship && type2 == Type::Asteroid) ||
		(type2 == Type::Ship && type1 == Type::Asteroid);
}

bool CollisionHandler::RecentlyCollide(GameObject firstObject, GameObject secondObject)
{
	int firstID = firstObject.GetID();
	int secondID = secondObject.GetID();

	for (auto it = recentCollisions.begin(); it != recentCollisions.end(); it++)
	{
		// Check if pairing did already collide 
		if ((it->firstObjectId == firstID && it->secondObjectId == secondID) || (it->firstObjectId == secondID && it->secondObjectId == firstID))
		{
			// Check if they recently collide (within 500 ms)
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

	// If collision pairing did not happen yet, create pairing
	CollisionOccurrence collisionOccurrence;
	collisionOccurrence.firstObjectId = firstID;
	collisionOccurrence.secondObjectId = secondID;
	collisionOccurrence.time = SDL_GetTicks();
	recentCollisions.push_back(collisionOccurrence);
	return false;
}

bool CollisionHandler::ShipAndBomb(const Type& type1, const Type& type2)
{
	return ((type1 == Type::Ship && type2 == Type::Bomb) ||
		(type2 == Type::Ship && type1 == Type::Bomb));
}

bool CollisionHandler::ShipAndFollower(const Type& type1, const Type& type2)
{
	return ((type1 == Type::Ship && type2 == Type::Follower) ||
		(type2 == Type::Ship && type1 == Type::Follower));
}

bool CollisionHandler::AsteroidAndAsteroid(const Type& type1, const Type& type2)
{
	return (type1 == Type::Asteroid && type2 == Type::Asteroid);
}

bool CollisionHandler::AsteroidAndShot(const Type& type1, const Type& type2)
{
	return (type1 == Type::Asteroid && type2 == Type::Shot) || 
		(type2 == Type::Asteroid && type1 == Type::Shot);
}

bool CollisionHandler::BombAndAsteroid(const Type& type1, const Type& type2)
{
	return (type1 == Type::Asteroid && type2 == Type::Bomb) ||
		(type2 == Type::Asteroid) && (type1 == Type::Bomb);
}

bool CollisionHandler::FollowerAndShot(const Type& type1, const Type& type2)
{
	return (type1 == Type::Follower && type2 == Type::Shot) ||
		(type2 == Type::Follower && type1 == Type::Shot);
}
