#include "gamesave.hpp"

GameSave::GameSave()
{
    std::ifstream file ("savegame.txt");
    std::string line;
    if (file.is_open())
    {
        while (getline (file, line))
        {
            highscore = std::stoi(line);
        }
    } else {
        std::cout << "Couldn't open. sorry :(" << std::endl;
    }
}

void GameSave::write()
{
    std::ofstream file;
    file.open ("savegame.txt", std::ios::out);
    file << highscore;
    file.close();
}
