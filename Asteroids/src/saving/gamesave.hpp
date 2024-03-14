#pragma once

#include <iostream>
#include <fstream>
#include <string>

class GameSave
{
public:
    GameSave();

private:
    int m_highscore = 0;
    int m_masterVolume = 50;
    int m_effectVolume = 50;
    int m_musicVolume = 50;
public:
    void WriteFile() const;
    int GetHighscore() const { return m_highscore; }
    float GetMasterVolume() const { return (m_masterVolume / 100.f); }
    float GetEffectVolume() const { return (m_effectVolume / 100.f); }
    float GetMusicVolume() const { return (m_musicVolume / 100.f); }

    void SetHighscore(int newHighscore) { m_highscore = newHighscore; }
    void SetMasterVolume(float masterVolume) { m_masterVolume = (int)(masterVolume * 100); }
    void SetEffectVolume(float masterVolume) { m_effectVolume = (int)(masterVolume * 100); }
    void SetMusicVolume(float masterVolume) { m_musicVolume = (int)(masterVolume * 100); }
};