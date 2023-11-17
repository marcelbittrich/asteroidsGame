#include "gamesave.hpp"

GameSave::GameSave()
{
	std::ifstream file("savegame.txt");
	std::string line;

	if (file.is_open())
	{
		int lineNmb = 0;
		while (getline(file, line))
		{
			if (lineNmb == 0) m_highscore = std::stoi(line);
			if (lineNmb == 1) m_masterVolume = std::stoi(line);
			lineNmb++;
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
	file << m_highscore << std::endl;
	file << m_masterVolume;
	file.close();
}
