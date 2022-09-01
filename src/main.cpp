#include "game.hpp"


Game *game = nullptr;

int main(int argc,char * argv[])
{

	const int FPS = 60;
	const float frameDelay = 1000.0f/FPS;

	Uint32 frameStart, frameTime;

	game = new Game();

	game->init("SDL-Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);

	while (game->running())
	{

		frameStart = SDL_GetTicks();

		game->handleEvents();
		game->update();
		game->render();

		frameTime = SDL_GetTicks() - frameStart;

		if(frameTime < frameDelay)
		{
			SDL_Delay((int)(frameDelay - frameTime));
		}
		else
		{
			SDL_Delay((int)frameDelay);
		}

	};
	
	game->clean();
	std::cout << "Na toll" << std::endl;

	return 0;
}