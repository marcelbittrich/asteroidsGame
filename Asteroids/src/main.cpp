#include "game.hpp"

int main(int argc, char* argv[])
{
	const int TargetFPS = 1000;
	const float frameCapTime = 1000.0f / TargetFPS;

	Game game;

	game.Init("Asteroids", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720);

	while (game.GetIsRunning())
	{
		Uint32 frameStart = SDL_GetTicks();
		game.HandleEvents();
		game.Update();
		Uint32 updateTime = SDL_GetTicks() - frameStart;
		game.Render();
		Uint32 renderTime = SDL_GetTicks() - updateTime - frameStart;
		Uint32 loopTime = SDL_GetTicks() - frameStart;

		if (loopTime < frameCapTime)
		{
			SDL_Delay((int)(frameCapTime - loopTime));
		}

		Uint32 frameTime = SDL_GetTicks() - frameStart;

		game.PrintPerformanceInfo(updateTime, renderTime, loopTime, frameTime);
	};

	game.Clean();

	return 0;
}