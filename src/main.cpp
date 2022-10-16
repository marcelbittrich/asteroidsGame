#include "game.hpp"


Game *game = nullptr;

// Performance assessment
bool showUpdateTime = false;
bool showRenderTime = false;
bool showFrameTime = false;
bool showDelayedFrameTime = false;
bool showFPS = true;

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
		Uint32 updateTime = SDL_GetTicks() - game->frameStart;
		game->render();
		Uint32 renderTime = SDL_GetTicks() - updateTime - game->frameStart;

		game->frameTime = SDL_GetTicks() - game->frameStart;

		if(game->frameTime < frameDelay)
		{
			SDL_Delay((int)(frameDelay - game->frameTime));
		}
		Uint32 delayedFrameTime = SDL_GetTicks() - game->frameStart;

		if (showUpdateTime) std::cout << "Update Time: " << updateTime << " ";
		if (showRenderTime) std::cout << "Render Time: " << renderTime << " ";
		if (showFrameTime) std::cout << "Frame Time: " << game->frameTime << " ";
		if (showDelayedFrameTime) std::cout << "Delayed Frame Time: " << delayedFrameTime << " ";
		if (showFPS) std::cout << "FPS: " << 1000/delayedFrameTime << " ";
 		if (showUpdateTime || showRenderTime|| showFrameTime || showDelayedFrameTime || showFPS) std::cout << std::endl;
	};
	
	game->clean();

	return 0;
}