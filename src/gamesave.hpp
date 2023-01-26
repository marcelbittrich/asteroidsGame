#include <iostream>
#include <fstream>
#include <string>

#ifndef GAMESAVE_HPP
#define GAMESAVE_HPP

class GameSave
{
public:
    unsigned highscore = 0;

    GameSave();
    void write();
};

#endif
