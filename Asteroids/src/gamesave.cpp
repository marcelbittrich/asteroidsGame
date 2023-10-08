#include "gamesave.hpp"

GameSave::GameSave()
{
	std::ifstream file("savegame.txt");
	std::string line;

	if (file.is_open())
	{
		while (getline(file, line))
		{
			m_highscore = std::stoi(line);
		}
	}
	else
	{
		std::cout << "Could not open savegame.txt" << std::endl;
	}
}

void GameSave::WriteFile()
{
	std::ofstream file;
	file.open("savegame.txt", std::ios::out);
	file << m_highscore;
	file.close();
}
