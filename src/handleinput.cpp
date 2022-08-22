#include "handleinput.hpp"

void handleInput(SDL_Event event, ControlBools *controlBools, bool * isRunning) {
    std::cout << "handle input: " << event.key.keysym.sym << std::endl;
    switch (event.type)
    {
        case SDL_QUIT:
            *isRunning = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
                case SDLK_UP:
                    controlBools->giveThrust = true;
                    break;
                case SDLK_DOWN:
                    break;
                case SDLK_RIGHT:
                    controlBools->isTurningRight = true;
                    break;
                case SDLK_LEFT:
                    controlBools->isTurningLeft = true;
                    break;
                default:
                    break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {
                case SDLK_UP:
                    controlBools->giveThrust = false;
                    break;
                case SDLK_DOWN:
                    break;
                case SDLK_RIGHT:
                    controlBools->isTurningRight = false;
                    break;
                case SDLK_LEFT:
                    controlBools->isTurningLeft = false;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}