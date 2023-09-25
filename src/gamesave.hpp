#pragma once

#include <iostream>
#include <fstream>
#include <string>

class GameSave
{

public:
    GameSave();

private:
    int highscore = 0;

public:
    void writeFile();
    int getHighscore() { return highscore; }
    void setHighscore(int newHighscore) { highscore = newHighscore; }
};