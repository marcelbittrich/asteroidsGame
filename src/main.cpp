#include "game.hpp"


Game *game = nullptr;

int main(int argc,char * argv[])
{

	const int FPS = 60;
	const float frameDelay = 1000.0f/FPS;

	game = new Game();

	game->init("SDL-Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);

	while (game->running())
	{

		game->frameStart = SDL_GetTicks();

		game->handleEvents();
		game->update();
		game->render();

		game->frameTime = SDL_GetTicks() - game->frameStart;

		if(game->frameTime < frameDelay)
		{
			SDL_Delay((int)(frameDelay - game->frameTime));
		}
	};
	
	game->clean();

	return 0;
}