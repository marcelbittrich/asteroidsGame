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

public:
    void WriteFile();
    int GetHighscore() { return m_highscore; }
    void SetHighscore(int newHighscore) { m_highscore = newHighscore; }
};