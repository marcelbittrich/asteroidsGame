#include "game.hpp"

SDL_Texture* playerTex;
SDL_Rect srcR, destR;

Game::Game()
{}
Game::~Game()
{}

void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
    int flags = 0;
    if(fullscreen){
        flags = SDL_WINDOW_FULLSCREEN;
    }

    if(SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        IMG_Init(IMG_INIT_PNG);
        std::cout << "Subsystem Initialised!..." << std::endl;

        window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
        if(window)
        {
            std::cout << "Window created" << std::endl;
        }

        renderer = SDL_CreateRenderer(window, -1, 0);
        if(renderer)
        {
            SDL_SetRenderDrawColor(renderer,255,0,0,255);
            std::cout << "Renderer created!" << std::endl;
        }
        isRunning = true;
    } else {
        isRunning = false;
    }
    SDL_Surface* image = IMG_Load("img/Png-logo.png");
    if (image == NULL) {
        std::cout << IMG_GetError();
    }
    playerTex = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);
    destR.x = 0;
    destR.y = 0;
    destR.w = 100;
    destR.h = 100;
}

void Game::handleEvents()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    switch (event.type)
    {
    case SDL_QUIT:
        isRunning = false;
        break;
    case SDL_KEYDOWN:
        std::cout << event.key.keysym.sym << std::endl;
        if (event.key.keysym.sym == SDLK_UP) {
            std::cout << "up" << std::endl;
            destR.y -= 5;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
            std::cout << "down" << std::endl;
            destR.y += 5;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
            std::cout << "up" << std::endl;
            destR.x += 5;
        }
        if (event.key.keysym.sym == SDLK_LEFT) {
            std::cout << "left" << std::endl;
            destR.x -= 5;
        }
        break;
    default:
        break;
    }
}

void Game::update()
{
}

void Game::render()
{
    SDL_RenderClear(renderer);
    //this is where we would add stuff to render
    SDL_RenderCopy(renderer, playerTex, NULL, &destR);
    SDL_RenderPresent(renderer);
}

void Game::clean()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    IMG_Quit();
    SDL_Quit();
    std::cout << "Game Cleaned" << std::endl;
}