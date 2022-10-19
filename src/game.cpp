#include "game.hpp"


Ship ship = Ship();

SDL_Texture* shipTex;
SDL_Texture* asteroidTexSmall;
SDL_Texture* asteroidTexMedium;
SDL_Texture* shotTex;


extern ControlBools controlBools;
int windowWidth, windowHeight;
int thrustAnimationCounter;
int currentThrustAnimationTime = 0;

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
            windowWidth = SDL_GetWindowSurface(window)->w;
            windowHeight = SDL_GetWindowSurface(window)->h;
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

    SDL_Surface* image = IMG_Load("../img/ship_thrustanimation.png");
    if (image == NULL) {
        std::cout << IMG_GetError();
    }
    shipTex = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("../img/asteroid_small1.png");
    asteroidTexSmall = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("../img/asteroid_medium1.png");
    asteroidTexMedium = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    image = IMG_Load("../img/shot.png");
    shotTex = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);


    gameBackground = background(windowWidth,windowHeight,100);



    ship = initShip(windowWidth, windowHeight);
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

    ship.update(controlBools,windowWidth,windowHeight);

    for(Asteroid &asteroid : asteroids)
    {
        asteroid.update(windowWidth,windowHeight);
    }

    for(const Asteroid &asteroid : asteroids)
    {
        if (doesCollide(ship,asteroid))
        {
            // std::cout << "Collision!!!!!!" << std::endl;
            ship.midPos[0] = windowWidth/2;
            ship.midPos[1] = windowHeight/2;
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
    for (Asteroid &asteroid : asteroids)
    {
        if (!asteroid.isVisible)
        {
            asteroid.isVisible = true;
            bool canStayVisible = true;
            for (Asteroid otherAsteroid : asteroids)
            {
                if (asteroid.id == otherAsteroid.id) continue;
                if (doesCollide(asteroid, otherAsteroid))
                {
                    canStayVisible = false;
                    break;
                }
            }
            if (!canStayVisible)
            {
                asteroid.isVisible = false;
            }
        }
    }
    

    //Make Shots
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
            if(timeSinceLastShot > 100){
                shoot(ship);
            }
        }
    }
    //Destroy Shots
 
    for (auto it = shots.begin(); it != shots.end(); it++)
    {   
        it->update(windowWidth, windowHeight);
    }


    if (!shots.empty())
    {
        auto firstShot = shots.begin();
        if (shotIsToOld(*firstShot))
        {
            shots.erase(firstShot);
        }
    }

    auto it = shots.begin();
    while (it != shots.end())
    {   
        bool hit = false;
        for (const Asteroid &asteroid : asteroids)
        {
            if (doesCollide(*it,asteroid))
            {
                it = shots.erase(it);
                hit = true;
                break;
            } 
        }
        if(!hit) it++;
    } 
    

    //Fill colObjects vector
    colObjects.push_back(ship);
    colObjects.insert(colObjects.end(),shots.begin(),shots.end()); 
    colObjects.insert(colObjects.end(),asteroids.begin(),asteroids.end());


    //Uint32 UpdateStart = SDL_GetTicks();
    gameBackground.update(colObjects);
    //Uint32 UpdateTime = SDL_GetTicks()- UpdateStart;

    //std::cout << UpdateTime << std::endl;

}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);

    gameBackground.render(renderer);   

    for(const Asteroid &asteroid: asteroids) {
        SDL_Texture* asteroidTex = nullptr;
        if (asteroid.size > 75) {
            asteroidTex = asteroidTexMedium;
        } else {
            asteroidTex = asteroidTexSmall;
        }
        if (asteroid.isVisible)
        {
            SDL_RenderCopyEx(renderer, asteroidTex, NULL, &asteroid.rect, 0.0f, NULL, SDL_FLIP_NONE);
        }
        SDL_SetRenderDrawColor(renderer,0,0,255,255);
        //drawCircle(renderer, asteroid.rect.x+asteroid.rect.w/2, asteroid.rect.y+asteroid.rect.h/2, round(asteroid.colRadius));
    }

    for (Shot &singleShot: shots)
    {
        SDL_SetRenderDrawColor(renderer,0,255,0,255);
        //drawCircle(renderer, singleShot.rect.x+singleShot.rect.w/2, singleShot.rect.y+singleShot.rect.h/2, round(singleShot.colRadius));
        singleShot.render(renderer,shotTex);
    }
    
    ship.render(renderer, shipTex);

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    //drawCircle(renderer, ship.rect.x+ship.rect.w/2, ship.rect.y+ship.rect.h/2, round(ship.colRadius));

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