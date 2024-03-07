#pragma once

#include <list>
#include <vector>

#include "SDL_stdinc.h"

#include "../objects/gameobjects/gameobject.hpp"


class CollisionHandler
{
public:
	CollisionHandler() {};
	CollisionHandler(class Game* owner) : m_game(owner) {};
	void CheckCollisions(const std::vector<GameObject*>& gameObjectPtrs);

	static bool DoesCollide(const GameObject& object1, const GameObject& object2);
private:
	void HandleShipAsteroidCollision(GameObject* object1, GameObject* object2);
	void HandleShipBombCollision(GameObject* object1, GameObject* object2);
	void HandleShipFollowerCollision(GameObject* object1, GameObject* object2);
	void HandleAsteroidAsteroidCollision(GameObject* object1, GameObject* object2);
	void HandleAsteroidShotCollision(GameObject* object1, GameObject* object2);
	void HandleBombAsteroidCollision(GameObject* object1, GameObject* object2);
	void HandleFollowerShotCollision(GameObject* object1, GameObject* object2);

	typedef GameObject::Type Type;
	bool ShipAndAnsteroid(const Type& type1, const Type& type2);
	bool ShipAndBomb(const Type& type1, const Type& type2);
	bool ShipAndFollower(const Type& type1, const Type& type2);
	bool AsteroidAndAsteroid(const Type& type1, const Type& type2);
	bool AsteroidAndShot(const Type& type1, const Type& type2);
	bool BombAndAsteroid(const Type& type1, const Type& type2);
	bool FollowerAndShot(const Type& type1, const Type& type2);

	struct CollisionOccurrence
	{
		int firstObjectId;
		int secondObjectId;
		Uint32 time;
	};
	bool RecentlyCollide(GameObject firstObject, GameObject secondObject);
	std::vector<CollisionOccurrence> recentCollisions = {};

	class Game* m_game = nullptr;
};