#pragma once

#include <list> 

#include "gameobject.hpp"

class Bomb : public Collectable
{
public:
	Bomb(Vec2 midPos, Vec2 velocity);
	void Update(float deltaTime) override;
	void Render() override;
	void GetCollected(class Ship* ownerShip) override;
	void Explode();

	static void SetTexture(struct SDL_Texture* texture) { s_texture = texture; }

	bool GetIsExploding() const { return isExploding; };

private:
	int m_size = 50;
	float m_colRadiusFactor = 0.3f;

	float m_explodindVelocity = 600.f;
	float m_maxExplodingRadius = 600.f;

	bool isExploding = false;
	bool isCollected = false;

	float m_rotation = 0.0f;
	float m_rotatingSpeed = 10.f;
	inline static struct SDL_Texture* s_texture;

	class Ship* m_ownerShip = nullptr;
};
