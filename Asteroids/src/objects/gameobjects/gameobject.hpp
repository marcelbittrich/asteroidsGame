#pragma once

#include "../../vector2.hpp"

struct GameObjectShared
{
	struct SDL_Renderer* renderer;
	class AudioPlayer* audioPlayer;
};

class GameObject
{
public:
	GameObject() : m_id(s_NewId++) {}
	GameObject(Vec2 midPos, Vec2 velocity) 
		: m_id(s_NewId++), m_midPos(midPos), m_velocity(velocity) {}

	static void ResetId() { s_NewId = 1; }
	virtual void Update(int windowWidth, int windowHeight, float deltaTime) {};
	virtual void Render() {};

	enum class Type {
		Default,
		Ship,
		Asteroid,
		Bomb,
		Shot
	};

	Type objectType = Type::Default;

	int GetID() const { return m_id; };
	int GetWidth() const { return m_width; };
	int GetHeight() const { return m_height; };
	float GetColRadius() const { return m_colRadius; };
	bool GetVisibility() const { return m_isVisible; };
	bool GetIsDead() const { return m_isDead; };
	Vec2 GetMidPos() const { return m_midPos; };
	Vec2 GetVelocity() const { return m_velocity; };

	void SetColRadius(float radius) { m_colRadius = radius; };
	void SetMidPos(Vec2 position) { m_midPos = position; };
	void SetVelocity(Vec2 velocity) { m_velocity = velocity; };
	void SetIsDead(bool value) { m_isDead = value; };

	static void SetRenderer(struct SDL_Renderer* renderer) { s_renderer = renderer; };
	static void SetAudioPlayer(class AudioPlayer* audioPlayer) { s_audioPlayer = audioPlayer; };

protected:
	int m_id			= -1;
	int m_width			= 0;
	int m_height		= 0;
	float m_colRadius	= 0.f;
	bool m_isVisible	= true;		// Indicator, false during respawn.
	bool m_isDead		= false;	// Can be destroyed.

	Vec2 m_midPos       = { 0, 0 }; 
	Vec2 m_velocity     = { 0, 0 };

	struct SDL_FRect GetRenderRect() const;
	inline static struct SDL_Renderer* s_renderer = nullptr;
	inline static class AudioPlayer* s_audioPlayer = nullptr;

private:
	inline static int s_NewId = 0;
};

Vec2 calcPosIfLeaving(Vec2 m_midPos, float radius, int windowWidth, int windowHeight);