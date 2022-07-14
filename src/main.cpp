#include "game.hpp"

Game *game = nullptr;

int main(int argc,char * argv[])
{
	game = new Game();

	game->init("SDL-Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);

	while (game->running())
	{
		game->handleEvents();
		game->update();
		game->render();

	};
	
	game->clean();

	std::cout << "Na toll";

	return 0;
}