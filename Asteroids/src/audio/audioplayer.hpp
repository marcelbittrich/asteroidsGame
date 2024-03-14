#pragma once

#include <vector>
#include <string>

#include "SDL_mixer.h"

enum class MusicType
{
	None,
	MenuMusic,
	GameMusic
};

enum class EffectType
{
	None,
	StartSound,
	EndSound,
	PauseOpen,
	PauseClose,
	ShotSound,
	SmallAsteroidExplode,
	BigAsteroidExplode,
	ThrusterSound,
	CollectedSound,
	BombExplode,
	ShipDeath
};

struct Music
{
	MusicType	type	= MusicType::None;
	Mix_Music*	sample	= nullptr;
};

struct Effect
{
	EffectType	type	= EffectType::None;
	Mix_Chunk*	sample	= nullptr;
};

class AudioPlayer
{
public:
	AudioPlayer();
	void PlayMusic(MusicType type);
	void PlaySoundEffect(EffectType type);
	void SetMasterVolume(float volume);
	float GetMasterVolume() const { return m_masterVolume; }

private:
	// master volume within 0.0..1.0
	float					m_masterVolume = 0.5f;
	std::vector<Music>		m_music = {};
	std::vector<Effect>		m_effects = {};
	Music createMusic(MusicType type, const std::string& path);
	Effect createEffect(EffectType type, const std::string& path, int defaultVolume);
	Music currentMusic;
};