#pragma once

#include "gameobject.hpp"

class PowerUp : public Collectable
{
public:
	PowerUp(Vec2 midPos, Vec2 velocity);
	void Update(float deltaTime) override;
	void Render() override;
	void GetCollected(class Ship* ownerShip) override;

	static void SetTexture(struct SDL_Texture* texture) { s_texture = texture; }

private:
	inline static struct SDL_Texture* s_texture;
	int m_size = 30;
	float m_rotation = 0.0f;
	float m_rotatingSpeed = 10.f;
	float m_colRadiusFactor = 0.5f;
};
