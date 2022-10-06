#include "game.hpp"


Ship ship = Ship();

SDL_Texture* playerTex;
SDL_Texture* thrustPlayerTex;
SDL_Texture* asteroidTexSmall;
SDL_Texture* asteroidTexMedium;
SDL_Texture* shotTex;
SDL_Rect srcR;

extern ControlBools controlBools;
int windowwidth, windowheight;
int thurstAnimationCounter = 0;

background gameBackground;

std::vector<double> velocity = {0.0, 0.0};
std::vector<Asteroid> asteroids;
std::vector<Shot> shots;
std::vector<Gameobject> colObjects;

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
            SDL_RenderClear(renderer);
            std::cout << "Renderer created!" << std::endl;
        }
        isRunning = true;
    } else {
        isRunning = false;
    }

    printf("%i joysticks were found.\n", SDL_NumJoysticks() );
    for (int i = 1; i <= SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            printf("Joystick %i is supported by the game controller interface!\n", i);
        }
    }
    printf("The names of the joysticks are:\n");
    for (int i=0; i < SDL_NumJoysticks(); i++ ){
        SDL_Joystick *joystick = SDL_JoystickOpen(i);
        printf("%s\n", SDL_JoystickName(joystick));
    }


    SDL_GameController *gamepad = nullptr;
    for (int i = 0; i < SDL_NumJoysticks(); ++i) {
        if (SDL_IsGameController(i)) {
            gamepad = SDL_GameControllerOpen(i);
            if (gamepad) {
                std::cout << "Gamecontroller opened!" << std::endl;
                break;
            } else {
                fprintf(stderr, "Could not open gamecontroller %i: %s\n", i, SDL_GetError());
            }
        }
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

    image = IMG_Load("img/asteroid_small1.png");
    asteroidTexSmall = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("img/asteroid_medium1.png");
    asteroidTexMedium = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("img/shot.png");
    shotTex = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);


    gameBackground = background(windowwidth,windowheight,100);

    srcR.w = 300;
    srcR.h = 300;
    srcR.x = 0;
    srcR.y = 0;
    double shipPosX = width/2-25;
    double shipPosY = height/2-25;


    ship = Ship(shipPosX, shipPosY, 50, 50);
    colObjects.push_back(ship);


    initAsteroids(ship.rect, width, height);
}

void Game::handleEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        handleInput(event, &controlBools, &isRunning);
    }

}

void Game::update()
{
    colObjects.clear();

    ship.update(controlBools,windowwidth,windowheight);

    thurstAnimationCounter = (thurstAnimationCounter + 1) % 3;
    srcR.x = thurstAnimationCounter * 300;
    //std::cout << thurstAnimationCounter << std::endl;
    for(Asteroid &asteroid : asteroids)
    {
        asteroid.update(windowwidth,windowheight);
    }

    for(Asteroid asteroid : asteroids)
    {
        if (doesCollide(ship,asteroid))
        {
            // std::cout << "Collision!!!!!!" << std::endl;
            ship.xPos = windowwidth/2-ship.rect.w/2;
            ship.yPos = windowheight/2-ship.rect.h/2;
            ship.velocity = {0,0};
        }
        
    }

    for(decltype(asteroids.size()) i = 0; i != asteroids.size(); i++)
    {
        for (decltype(asteroids.size()) j = i+1; j != asteroids.size(); j++)
            {
                //std::cout << "Kombination: " << i << ", " << j << std::endl;
                asteroidsCollide(asteroids[i],asteroids[j]);
            } 
    }

    if (controlBools.isShooting)
    {
        if (shots.empty())
        {
            shoot(ship);
        } else 
        {    
            auto lastShot = shots.end()-1;
            Shot lastShotEnt = *lastShot;
            Uint32 timeSinceLastShot;
            timeSinceLastShot = SDL_GetTicks() - lastShotEnt.creationTime;
            if(timeSinceLastShot > 50){
                shoot(ship);
            }
        }
    }
    
    for (Shot &singleShot : shots)
    {   
        singleShot.update(windowwidth, windowheight);
    }
    if (!shots.empty())
    {
        auto firstShot = shots.begin();
        if (shotIsToOld(*firstShot))
        {
            shots.erase(firstShot);
        }
    }

    colObjects.push_back(ship);
    colObjects.insert(colObjects.end(),asteroids.begin(),asteroids.end());
    // gameBackground.update(colObjects);

}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    //this is where we would add stuff to render

    //gameBackground.render(renderer);

    if (controlBools.giveThrust) {
        SDL_RenderCopyEx(renderer, thrustPlayerTex, &srcR, &ship.rect, ship.shipAngle, NULL, SDL_FLIP_NONE);
    } else {
        SDL_RenderCopyEx(renderer, playerTex, NULL, &ship.rect, ship.shipAngle, NULL, SDL_FLIP_NONE);
    }
    for(Asteroid asteroid: asteroids) {
        SDL_Texture* asteroidTex = nullptr;
        if (asteroid.size > 75) {
            asteroidTex = asteroidTexMedium;
        } else {
            asteroidTex = asteroidTexSmall;
        }
        SDL_RenderCopyEx(renderer, asteroidTex, NULL, &asteroid.rect, 0.0f, NULL, SDL_FLIP_NONE);
        SDL_SetRenderDrawColor(renderer,0,0,255,255);
        drawcircle(renderer, asteroid.rect.x+asteroid.rect.w/2, asteroid.rect.y+asteroid.rect.h/2, round(asteroid.col_radius));
    }

    for (Shot singleShot: shots)
    {
        SDL_SetRenderDrawColor(renderer,0,255,0,255);
        //drawcircle(renderer, singleShot.rect.x+singleShot.rect.w/2, singleShot.rect.y+singleShot.rect.h/2, round(singleShot.col_radius));
        singleShot.render(renderer,shotTex);
    }
    

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    drawcircle(renderer, ship.rect.x+ship.rect.w/2, ship.rect.y+ship.rect.h/2, round(ship.col_radius));

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