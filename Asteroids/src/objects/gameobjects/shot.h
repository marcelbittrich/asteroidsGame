#pragma once

#include <list>

#include "gameobject.hpp"

class Shot : public Weapon
{
public:
	Shot(Vec2 midPos, Vec2 velocity, float rotation);
	void Update(float deltaTime)  override;
	void Render() override;

	static void SetTexture(struct SDL_Texture* texture) { s_texture = texture; }
private:
	float m_rotation = 0.f;

	inline static SDL_Texture* s_texture;

	int m_size = 40;
	float m_colRadiusFactor = 0.3f;

	float m_lifeTimeInSeconds = 0.f;
	float m_maxLifeTimeInSeconds = 0.8f;
	bool IsTooOld(float deltaTime);
};
