#include <iostream>
#include <fstream>
#include <string>

#ifndef GAMESAVE_HPP
#define GAMESAVE_HPP

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

#endif
