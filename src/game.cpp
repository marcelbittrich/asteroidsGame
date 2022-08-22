#include "game.hpp"
#include "handleinput.hpp"

SDL_Texture* playerTex;
SDL_Texture* thrustPlayerTex;
SDL_Rect srcR, destR;
ControlBools controlBools;

double shipAngle = 0.0;
double shipPosX = 0.0;
double shipPosY = 0.0;
double v_sum = 0.0;
double v_max = 20;
double roatatingSpeed = 2;
double thrust = 0.05;

int windowwidth, windowheight;

int thurstAnimationCounter = 0;

std::vector<double> velocity = {0.0, 0.0};


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

    image = IMG_Load("img/ship_thrustanimation.png");
    thrustPlayerTex = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    destR.w = 50;
    destR.h = 50;
    srcR.w = 300;
    srcR.h = 300;
    srcR.x = 0;
    srcR.y = 0;
    shipPosX = width/2-destR.w/2;
    shipPosY = height/2-destR.h/2;
}

void Game::handleEvents()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    handleInput(event, &controlBools, &isRunning);
}

void Game::update()
{

    double deltaX = 0;
    double deltaY = 0; 
    double v_angle = 0;
    double v_angle_degree = 0; 

    v_sum = sqrt(std::pow(velocity[0],2) + std::pow(velocity[1],2));

    if (v_sum > 0)
    {
        v_angle = atan2(velocity[0],velocity[1]);
        v_sum = std::max(v_sum - 0.01, 0.0);
    }    

    v_angle_degree = v_angle*180/PI;
    std::cout << "direction heading: " << v_angle_degree << std::endl;

    
    velocity.at(0) = (sin(v_angle) * v_sum);
    velocity.at(1) = (cos(v_angle) * v_sum); 

    if (controlBools.giveThrust)
    {


        if (v_sum <= v_max)
        {
            deltaX = (sin(shipAngle*PI/180) * thrust);
            deltaY = -(cos(shipAngle*PI/180) * thrust); 
        }
        
    }

    velocity.at(0) += deltaX;
    velocity.at(1) += deltaY;


    if (controlBools.isTurningRight)
    {
        shipAngle += roatatingSpeed;
    }

    if (controlBools.isTurningLeft)
    {
        shipAngle -= roatatingSpeed;
    }

    shipPosX += velocity.at(0);
    shipPosY += velocity.at(1);
    // std::cout <<"Updated velocity: "<< velocity << " xPos: " << destR.x << " yPos: " << destR.y << std::endl;



    if (shipPosX < 0){
        shipPosX = windowwidth;
    }
    if (shipPosX > windowwidth){
        shipPosX = 0;
    }
    if (shipPosY < 0){
        shipPosY = windowheight;
    }
    if (shipPosY > windowheight){
        shipPosY = 0;
    }

    destR.x = std::round(shipPosX);
    destR.y = std::round(shipPosY);

    thurstAnimationCounter = (thurstAnimationCounter + 1) % 3;
    srcR.x = thurstAnimationCounter * 300;
}

void Game::render()
{
    SDL_RenderClear(renderer);
    //this is where we would add stuff to render
    if (controlBools.giveThrust) {
        SDL_RenderCopyEx(renderer, thrustPlayerTex, &srcR, &destR, shipAngle, NULL, SDL_FLIP_NONE);
    } else {
        SDL_RenderCopyEx(renderer, playerTex, NULL, &destR, shipAngle, NULL, SDL_FLIP_NONE);
    }
    SDL_RenderDrawLine(renderer, shipPosX, shipPosY, 0, 0);
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