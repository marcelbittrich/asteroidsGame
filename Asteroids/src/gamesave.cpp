#include "gamesave.hpp"

GameSave::GameSave()
{
	std::ifstream file("savegame.txt");
	std::string line;

	if (file.is_open())
	{
		while (getline(file, line))
		{
			highscore = std::stoi(line);
		}
	}
	else
	{
		std::cout << "Could not open savegame.txt" << std::endl;
	}
}

void GameSave::writeFile()
{
	std::ofstream file;
	file.open("savegame.txt", std::ios::out);
	file << highscore;
	file.close();
}
