#include "game.hpp"

SDL_Texture* playerTex;
SDL_Rect srcR, destR;
double velocity = 0;
double v_max = 5;
double shipAngle = 0;
double roatatingSpeed = 3;
int windowwidth, windowheight;

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
            windowwidth = SDL_GetWindowSurface(window)->w;
            windowheight = SDL_GetWindowSurface(window)->h;
        }

        renderer = SDL_CreateRenderer(window, -1, 0);
        if(renderer)
        {
            SDL_SetRenderDrawColor(renderer,0,0,0,0);
            std::cout << "Renderer created!" << std::endl;
        }
        isRunning = true;
    } else {
        isRunning = false;
    }
    SDL_Surface* image = IMG_Load("img/ship.png");
    if (image == NULL) {
        std::cout << IMG_GetError();
    }
    playerTex = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);
    destR.w = 50;
    destR.h = 50;
    destR.x = width/2-destR.w/2;
    destR.y = height/2-destR.h/2;
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
            if(velocity<v_max){
            velocity += 1.5;
            }

            std::cout << velocity << std::endl;
        }
        if (event.key.keysym.sym == SDLK_DOWN) {
            std::cout << "down" << std::endl;
        }
        if (event.key.keysym.sym == SDLK_RIGHT) {
            std::cout << "right" << std::endl;
            shipAngle += roatatingSpeed;
            std::cout << shipAngle << std::endl;
        }
        if (event.key.keysym.sym == SDLK_LEFT) {
            std::cout << "left" << std::endl;
            shipAngle -= roatatingSpeed;
            std::cout << shipAngle << std::endl;
        }
        if (event.key.keysym.sym == SDLK_SPACE) {
            std::cout << "position reset" << std::endl;

            
            std::cout << windowheight << windowwidth << std::endl; 
            destR.x = windowwidth/2-destR.w/2;
            destR.y = windowheight/2-destR.h/2;
        }
        break;
    default:
        break;
    }
}

void Game::update()
{
if (velocity>0)
{
    velocity -= 0.05;
    destR.x += (sin(shipAngle*PI/180)*velocity);
    destR.y += -(cos(shipAngle*PI/180)*velocity);
    std::cout <<"Updated velocity: "<< velocity << " xPos: " << destR.x << " yPos: " << destR.y << std::endl;

    if (destR.x < 0){
        destR.x = windowwidth;
    }
    if (destR.x > windowwidth){
        destR.x = 0;
    }
    if (destR.y < 0){
        destR.y = windowheight;
    }
    if (destR.y > windowheight){
        destR.y = 0;
    }
} 

}

void Game::render()
{
    SDL_RenderClear(renderer);
    //this is where we would add stuff to render
    SDL_RenderCopyEx(renderer, playerTex, NULL, &destR, shipAngle, NULL, SDL_FLIP_NONE);
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