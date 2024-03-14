#include "audioplayer.hpp"

#include <iostream>
#include <algorithm>


AudioPlayer::AudioPlayer()
{
	m_music.push_back(createMusic(MusicType::GameMusic, "./sfx/music/newbattle.wav"));
	m_music.push_back(createMusic(MusicType::MenuMusic, "./sfx/music/spiteful_fight.mp3"));
	Mix_VolumeMusic((int)(m_masterVolume * MIX_MAX_VOLUME));

	m_effects.push_back(createEffect(EffectType::ShotSound, "./sfx/effects/smallLaser_01.ogg", MIX_MAX_VOLUME / 2));
	m_effects.push_back(createEffect(EffectType::SmallAsteroidExplode, "./sfx/effects/smallAsteroidExplode_01.ogg", (int)(MIX_MAX_VOLUME / 1.5)));
	m_effects.push_back(createEffect(EffectType::BigAsteroidExplode, "./sfx/effects/bigAsteroidExplode_01.ogg", MIX_MAX_VOLUME ));
	m_effects.push_back(createEffect(EffectType::StartSound, "./sfx/effects/start_01.ogg", MIX_MAX_VOLUME / 2));
	m_effects.push_back(createEffect(EffectType::EndSound, "./sfx/effects/end_01.ogg", MIX_MAX_VOLUME / 2));
	m_effects.push_back(createEffect(EffectType::PauseOpen, "./sfx/effects/pauseOpen_01.ogg", MIX_MAX_VOLUME / 4));
	m_effects.push_back(createEffect(EffectType::PauseClose, "./sfx/effects/pauseClose_01.ogg", MIX_MAX_VOLUME / 4));
	m_effects.push_back(createEffect(EffectType::ThrusterSound, "./sfx/effects/thruster_01.ogg", MIX_MAX_VOLUME / 2));
	m_effects.push_back(createEffect(EffectType::CollectedSound, "./sfx/effects/collected_01.ogg", MIX_MAX_VOLUME / 2));
	m_effects.push_back(createEffect(EffectType::BombExplode, "./sfx/effects/bombExplode_01.ogg", MIX_MAX_VOLUME / 2));
	m_effects.push_back(createEffect(EffectType::ShipDeath, "./sfx/effects/shipDeath_01.ogg", MIX_MAX_VOLUME));

	// Allocate 8 mixing channels.
	Mix_AllocateChannels(8);

	// Set master volume to default volume.
	SetMasterVolume(m_masterVolume);
}

void AudioPlayer::PlayMusic(MusicType type)
{
	for (const Music& music : m_music)
	{
		if (music.type == type && currentMusic.type != type)
		{
			currentMusic.type = type;
			if (Mix_FadeInMusic(music.sample, -1, 2000) == -1)
				std::cout << "Mix_PlayMusic: " << Mix_GetError() << std::endl;
		}
	}
}

void AudioPlayer::PlaySoundEffect(EffectType type)
{
	for (const Effect& effect: m_effects) 
	{
		if (effect.type == type)
		{
			if (Mix_PlayChannel(-1, effect.sample, 0) == -1)
				std::cout << "Mix_PlayChannel: " << Mix_GetError() << std::endl;
		}
	}
}

void AudioPlayer::SetMasterVolume(float volume)
{
	m_masterVolume = std::clamp(volume, 0.f, 1.f);
	Mix_MasterVolume((int)(m_masterVolume * MIX_MAX_VOLUME));
	Mix_VolumeMusic((int)(m_masterVolume * MIX_MAX_VOLUME));
}

Music AudioPlayer::createMusic(MusicType type, const std::string& path)
{
	Music newMusic;
	newMusic.sample = Mix_LoadMUS(path.c_str());
	newMusic.type = type;
	return newMusic;
}

Effect AudioPlayer::createEffect(EffectType type, const std::string& path, int defaultVolume)
{
	Effect newEffect;
	newEffect.sample = Mix_LoadWAV(path.c_str());
	newEffect.type = type;
	Mix_VolumeChunk(newEffect.sample, defaultVolume);
	return newEffect;
}
