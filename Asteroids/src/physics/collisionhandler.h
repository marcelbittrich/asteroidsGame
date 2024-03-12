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
	void MoveApart(GameObject& object1, GameObject& object2);
	static bool DoesCollide(const GameObject& object1, const GameObject& object2);
private:
	void HandleElasticCollision(GameObject* object1, GameObject* object2);

	typedef GameObject::Type Type;
	typedef GameObject::CollisionType CollisionType;

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