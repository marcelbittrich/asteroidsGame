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
public:
    void WriteFile();
    int GetHighscore() { return m_highscore; }
    void SetHighscore(int newHighscore) { m_highscore = newHighscore; }
    float GetMasterVolume() { return (m_masterVolume / 100.f); }
    void SetMasterVolume(float masterVolume) { m_masterVolume = (int)(masterVolume * 100); }
};