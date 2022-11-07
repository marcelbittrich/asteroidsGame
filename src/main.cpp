#include "game.hpp"


Game *game = nullptr;

// Performance assessment
bool showUpdateTime = false;
bool showRenderTime = false;
bool showFrameTime = false;
bool showDelayedFrameTime = false;
bool showFPS = false;

int main(int argc,char * argv[])
{

	const int FPS = 60;
	const float frameCapTime = 1000.0f/FPS;

	game = new Game();

	game->init("SDL-Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);

	while (game->running())
	{

		game->frameStart = SDL_GetTicks();

		game->handleEvents();
		game->update(); 
		Uint32 updateTime = SDL_GetTicks() - game->frameStart;
		game->render();
		Uint32 renderTime = SDL_GetTicks() - updateTime - game->frameStart;

		Uint32 loopTime = SDL_GetTicks() - game->frameStart;

		if(loopTime < frameCapTime)
		{
			SDL_Delay((int)(frameCapTime - loopTime));
		}

		game->frameTime = SDL_GetTicks() - game->frameStart;

		if (showUpdateTime) std::cout << "Update Time: " << updateTime << " ";
		if (showRenderTime) std::cout << "Render Time: " << renderTime << " ";
		if (showFrameTime) std::cout << "Frame Time: " << loopTime  << " ";
		if (showDelayedFrameTime) std::cout << "Delayed Frame Time: " << game->frameTime << " ";
		if (showFPS) std::cout << "FPS: " << 1000/game->frameTime << " ";
 		if (showUpdateTime || showRenderTime|| showFrameTime || showDelayedFrameTime || showFPS) std::cout << std::endl;
	};
	
	game->clean();

	return 0;
}