#include "game.hpp"


Ship ship = Ship();

SDL_Texture* shipTex;
SDL_Texture* asteroidTexSmall;
SDL_Texture* asteroidTexMedium;
SDL_Texture* shotTex;
SDL_Texture* bombTex;
TTF_Font* font;

extern ControlBools controlBools;
int windowWidth, windowHeight;
int thrustAnimationCounter;
int currentThrustAnimationTime = 0;


background gameBackground;

std::vector<double> velocity = {0.0, 0.0};
std::list<GameObject> colObjects;

UICounter UIScore;
UICounter UILife;
UICounter UIBomb;
UICounter UIFPS;

unsigned score;
unsigned life;
unsigned bombCount;
unsigned FPS;
std::vector<int> FPSVector;

float timeSinceLastAsteroid = 0;
int difficulty = 1;

Uint32 lastUpdateTime = SDL_GetTicks();

ShotMeter shotMeter;

bool newClick = true;
bool newBombIgnition = true;

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
        if(TTF_Init()) std::cout << "Font System Initialised!...";

        window = SDL_CreateWindow(title, xpos, ypos, width, height, SDL_WINDOW_RESIZABLE);
        if(window)
        {
            std::cout << "Window created" << std::endl;
            windowWidth = 1280;
            windowHeight = 720;
        }

        renderer = SDL_CreateRenderer(window, -1, 0);
        if(renderer)
        {
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
            SDL_RenderSetLogicalSize(renderer,windowWidth,windowHeight);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
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

    image = IMG_Load("../img/bomb.png");
    bombTex = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    gameBackground = background(windowWidth,windowHeight,100);

    ship = initShip(windowWidth, windowHeight);
    colObjects.push_back(ship);

    initAsteroids(ship, windowWidth, windowHeight);

    font = TTF_OpenFont("../font/joystix_monospace.ttf", 20);

    score = 0;
    life = 5;
    bombCount = 0;

    lastUpdateTime = SDL_GetTicks();

    shotMeter = ShotMeter(&ship, 0, 25, 40, 6);
    SDL_Color white = {255, 255, 255, 255};
    UIScore = UICounter("Score", font, white, windowWidth, windowHeight, 32, 16, UICounterPosition::Left, true);
    UILife = UICounter("Lifes", font, white, windowWidth, windowHeight, 32, 16, UICounterPosition::Right, true);
    UIBomb = UICounter("Bombs", font, white, windowWidth, windowHeight, 32, 16, UICounterPosition::Right, true);
    UIFPS = UICounter("FPS", font, white, windowWidth, windowHeight, 32, 16, UICounterPosition::Right, true);
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
    if(life == 0) return;
    
    Uint32 currentTime = SDL_GetTicks();
    float deltaTime =  (currentTime - lastUpdateTime) / 1000.0f;
    lastUpdateTime = currentTime;

    if (controlBools.isLeftClicking && newClick)
    {
        newClick = false;
        int mouseXPos, mouseYPos;
        SDL_GetMouseState(&mouseXPos, &mouseYPos);
        std::cout << "Left Click at: " << mouseXPos << " " << mouseYPos << std::endl;
        Bomb(mouseXPos, mouseYPos, getRandomVelocity(0.0f, 0.5f));
    } else if (!controlBools.isLeftClicking)
    {
        newClick = true;
    }

    if (controlBools.isUsingBomb && newBombIgnition)
    {
        newBombIgnition = false;
        if (!Bomb::pCollectedBombs.empty())
        {
            auto it = Bomb::pCollectedBombs.begin();
            (*it)->explode();
            Bomb::pCollectedBombs.erase(it);
        }
    } else if (!controlBools.isUsingBomb)
    {
        newBombIgnition = true;
    }

    bombCount = Bomb::pCollectedBombs.size();

    // Update Ship
    ship.update(controlBools, windowWidth, windowHeight, &deltaTime);

    // Update Asteroid Position
    for(Asteroid &asteroid : Asteroid::asteroids)
    {
        asteroid.update(windowWidth, windowHeight, &deltaTime);
    }

    // Check Ship Collision Asteroids
    for(const Asteroid &asteroid : Asteroid::asteroids)
    {
        if (doesCollide(ship, asteroid))
        {
            if(--life == 0) break;
            ship.respawn(renderer);  
        }
        
    }

    // Check Ship Collision Asteroids
    for(Bomb &bomb : Bomb::bombs)
    {
        if (!bomb.isExploding && doesCollide(ship, bomb))
        {
            bomb.collect(); 
        }    
    }

    // Spawn New Asteroids

    timeSinceLastAsteroid += deltaTime;
    if (timeSinceLastAsteroid >= 5)
    {
        timeSinceLastAsteroid = 0;

        std::cout << "Spawn small asteroid" << std::endl;      
        initSingleAsteroid (colObjects, windowWidth, windowHeight, AsteroidSizeType::Small);
        if (difficulty % 5 == 0)
        {
            std::cout << "Spawn medium asteroid" << std::endl; 
            initSingleAsteroid (colObjects, windowWidth, windowHeight, AsteroidSizeType::Medium);
        } 
        
        difficulty++;
    }


    // Check Asteroid Collision
    for(auto it1 = Asteroid::asteroids.begin(); it1 != Asteroid::asteroids.end(); it1++)
    {
        for (auto it2 = std::next(it1, 1); it2 != Asteroid::asteroids.end(); it2++)
        {
            //std::cout << "Kombination: " << i << ", " << j << std::endl;
            asteroidsCollide(*it1, *it2);
        }
        
    }



    for (Asteroid &asteroid : Asteroid::asteroids)
    {
        if (!asteroid.isVisible)
        {
            asteroid.isVisible = true;
            bool canStayVisible = true;
            for (Asteroid otherAsteroid : Asteroid::asteroids)
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
    if (controlBools.isShooting){
        ship.shoot();
    }

    shotMeter.update(ship.getShotCounter(), ship.getMaxShotCounter(), &ship);

    //Update Shots
    for (Shot &singleShot: Shot::shots)
    {
        singleShot.update(windowWidth, windowHeight, &deltaTime);
    }

    //Destroy Shots
    if (!Shot::shots.empty())
    {
        auto firstShot = Shot::shots.begin();
        if (shotIsToOld(*firstShot))
        {
            Shot::shots.erase(firstShot);
        }
    }

    auto it = Shot::shots.begin();
    while (it != Shot::shots.end())
    {   
        bool hit = false;

        auto asteroidIt = Asteroid::asteroids.begin();
        while (asteroidIt != Asteroid::asteroids.end())
        {
            if (doesCollide(*it, *asteroidIt))
            {
                it = Shot::shots.erase(it);

                hit = true;

                if (asteroidIt->sizeType == AsteroidSizeType::Small)
                {
                    asteroidIt = Asteroid::asteroids.erase(asteroidIt);
                    score++;
                    break;
                }
                else if (asteroidIt->sizeType == AsteroidSizeType::Medium)
                {   
                    handleDistruction(*asteroidIt);  
                    asteroidIt = Asteroid::asteroids.erase(asteroidIt);
                    break;
                }

                break;
            }
            if (!hit) asteroidIt++;
        }

        if(!hit) it++;
    } 

    // Update Bombs
    for (auto bombIt =  Bomb::bombs.begin(); bombIt != Bomb::bombs.end(); bombIt++)
    {
        bombIt->update(windowWidth, windowHeight, &deltaTime, &ship);

        if (bombIt->isExploding)
        {   
            bool hit = false;
            auto asteroidIt = Asteroid::asteroids.begin();
            while (asteroidIt != Asteroid::asteroids.end())
            {
                std::cout << "Bomb colRadius: " << bombIt->colRadius << std::endl;  
                if (doesCollide(*bombIt, *asteroidIt))
                {
                    hit = true;
                    asteroidIt = Asteroid::asteroids.erase(asteroidIt);
                }   
                else
                {
                    hit = false;
                }
                if (!hit) asteroidIt++;
            }
        }
        
    }

    // Destoy Bombs
    if (!Bomb::bombs.empty())
    {   
        auto bombIt = Bomb::bombs.begin();
        while (bombIt != Bomb::bombs.end())
        {
            bool didErase = false;
            if (bombIt->isDead)
            {
                bombIt = Bomb::bombs.erase(bombIt);
                didErase = true;
            }
            if (!didErase) bombIt++;
        }
    }

    //Fill colObjects vector
    colObjects.clear();
    colObjects.push_back(ship);
    colObjects.insert(colObjects.end(), Shot::shots.begin(), Shot::shots.end()); 
    colObjects.insert(colObjects.end(), Asteroid::asteroids.begin(), Asteroid::asteroids.end());
    colObjects.insert(colObjects.end(), Bomb::bombs.begin(), Bomb::bombs.end());

    gameBackground.update(colObjects, &deltaTime);

    if (frameTime != 0)
    {
        FPS = 1000/frameTime;
        if (FPSVector.size() >= 10)
        {
            FPSVector.insert(FPSVector.begin(), FPS);
            FPSVector.pop_back();
        } 
        else
        {
            FPSVector.insert(FPSVector.begin(), FPS);
        }   
    } else
    {
        FPS = 0;
    }

    int averageFPS;
    if (!FPSVector.empty())
    {
        auto count = (float)(FPSVector.size());
        averageFPS = std::reduce(FPSVector.begin(), FPSVector.end())/count;
    } else
    {
        averageFPS = 0;
    }
    
    UIScore.update(score, renderer);

    UIFPS.update(averageFPS, renderer);
    UILife.update(life, renderer);

    UIBomb.update(Bomb::pCollectedBombs.size(), renderer);
}

void Game::render()
{
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);
    SDL_Rect playArea = {0, 0, windowWidth, windowHeight};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &playArea);

    gameBackground.render(renderer);   

    for (Bomb &bomb: Bomb::bombs)
    {
        bomb.render(renderer, bombTex);
    }

    for(Asteroid &asteroid: Asteroid::asteroids) {
        asteroid.render(renderer, asteroidTexSmall, asteroidTexMedium);
        // SDL_SetRenderDrawColor(renderer,0,0,255,255);
        // drawCircle(renderer, asteroid.rect.x+asteroid.rect.w/2, asteroid.rect.y+asteroid.rect.h/2, round(asteroid.colRadius));
    }

    for (auto it = Shot::shots.begin(); it != Shot::shots.end(); it ++)
    {
        it->render(renderer,shotTex);
        // SDL_SetRenderDrawColor(renderer,0,255,0,255);
        // drawCircle(renderer, singleShot.rect.x+singleShot.rect.w/2, singleShot.rect.y+singleShot.rect.h/2, round(singleShot.colRadius));
    }

    ship.render(renderer, shipTex);
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    //drawCircle(renderer, ship.rect.x+ship.rect.w/2, ship.rect.y+ship.rect.h/2, round(ship.colRadius));

    UIScore.render(renderer);
    UILife.render(renderer);
    UIBomb.render(renderer);
    UIFPS.render(renderer);

    // ShotMeter
    shotMeter.render(renderer, ship.canShoot);

    if (life == 0) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
        SDL_RenderFillRect(renderer, &playArea);
    }

    SDL_RenderPresent(renderer);
}

void Game::clean()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    TTF_CloseFont(font);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    std::cout << "Game Cleaned" << std::endl;
}