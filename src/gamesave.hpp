#include <iostream>
#include <fstream>
#include <string>

#ifndef GAMESAVE_HPP
#define GAMESAVE_HPP

class GameSave
{
public:
    GameSave();
    void write();
    int highscore = 0;
};

#endif
