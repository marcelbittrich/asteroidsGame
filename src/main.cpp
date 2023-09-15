#include "game.hpp"
#include "gamesave.hpp"

Game *game;

int main(int argc, char *argv[])
{

	const int TargetFPS = 1000;
	const float frameCapTime = 1000.0f / TargetFPS;

	game = new Game();
	game->gameState = STATE_IN_MENU;

	game->init("Asteroid Game Project", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);

	while (game->getIsRunning())
	{
		if (game->gameState == STATE_RESET)
		{
			game->reset();
		}
		else
		{
			game->frameStart = SDL_GetTicks();

			game->handleEvents();
			game->update();
			Uint32 updateTime = SDL_GetTicks() - game->frameStart;
			game->render();
			Uint32 renderTime = SDL_GetTicks() - updateTime - game->frameStart;

			Uint32 loopTime = SDL_GetTicks() - game->frameStart;

			if (loopTime < frameCapTime)
			{
				SDL_Delay((int)(frameCapTime - loopTime));
			}

			game->frameTime = SDL_GetTicks() - game->frameStart;

			game->printPerformanceInfo(updateTime, renderTime, loopTime);
		}
	};

	game->clean();

	return 0;
}