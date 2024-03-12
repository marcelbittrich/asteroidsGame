#include "collisionhandler.h"

#include <iostream>
#include "math.h"

#include "../vector2.hpp"
#include "../game.hpp"
#include "../objects/helper.hpp"
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
				const CollisionType type1 = object1->colType;
				const CollisionType type2 = object2->colType;

				Ship* ship = nullptr;
				Enemy* enemy = nullptr;
				Collectable* collectable = nullptr;
				Weapon* weapon = nullptr;

				// Check who is who

				if (type1 == CollisionType::Ship)
					ship = (Ship*)object1;
				else if (type2 == CollisionType::Ship)
					ship = (Ship*)object2;

				if (type1 == CollisionType::Enemy && type1 == type2)
				{
					HandleElasticCollision(object1, object2);
					MoveApart(*object1, *object2);
				}

				if (type1 == CollisionType::Enemy)
					enemy = (Enemy*)object1;
				if (type2 == CollisionType::Enemy)
					enemy = (Enemy*)object2;

				if (type1 == CollisionType::Collectable)
					collectable = (Collectable*)object1;
				if (type2 == CollisionType::Collectable)
					collectable = (Collectable*)object2;

				if (type1 == CollisionType::Weapon)
					weapon = (Weapon*)object1;
				if (type2 == CollisionType::Weapon)
					weapon = (Weapon*)object2;

				// React on collision type

				if (ship && enemy)
				{
					ship->Respawn();
					Game::DecreaseLife();
				} 
				else if (ship && collectable)
				{
					collectable->GetCollected(ship);
				}

				if (weapon && enemy)
				{
					enemy->HandleDestruction();

					if (m_game->GetScore() % BOMB_SPAWN_ON_SCORE == 0)
					{
						Bomb newBomb = Bomb(enemy->GetMidPos(), GetRandomVelocity(0.0f, 0.5f));
						Game::bombs.push_back(newBomb);
					}
					m_game->IncreaseScore();

					enemy->SetIsDead(true);
					if (weapon->objectType != Type::Bomb)
					{
						weapon->SetIsDead(true);
					}
				}
			}
		}
	}
}

void CollisionHandler::MoveApart(GameObject& object1, GameObject& object2)
{
	Vec2 distance = object2.GetMidPos() - object1.GetMidPos();

	if (object2.GetColRadius() == 0)
		return;

	float intrusionDepth = object1.GetColRadius() + object2.GetColRadius() - distance.Length();
	Vec2 hitPosition = object1.GetMidPos() + distance.Normalize() * (object1.GetColRadius() - intrusionDepth / 2.f);

	Vec2 newPositionOb1 = hitPosition - distance.Normalize() * object1.GetColRadius();
	Vec2 newPositionOb2 = hitPosition + distance.Normalize() * object2.GetColRadius();

	object1.SetMidPos(newPositionOb1);
	object2.SetMidPos(newPositionOb2);
}

void CollisionHandler::HandleElasticCollision(GameObject* object1, GameObject* object2)
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