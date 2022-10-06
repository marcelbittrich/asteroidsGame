#include "handleinput.hpp"

void handleInput(SDL_Event event, ControlBools *controlBools, bool * isRunning) {
    
    //std::cout << "handle keyboard input: " << event.key.keysym.sym << std::endl;
    //std::cout << "controller input" << event.type << std::endl;

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
                case SDLK_SPACE:
                    controlBools->isShooting = true;
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
                case SDLK_SPACE:
                    controlBools->isShooting = false;
                default:
                    break;
            }
            break;
  
        case SDL_CONTROLLERBUTTONDOWN:
            switch (event.cbutton.button)
            {
                case SDL_CONTROLLER_BUTTON_B:
                    controlBools->giveThrust = true;
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    controlBools->isTurningRight = true;
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    controlBools->isTurningLeft = true;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                    controlBools->isShooting = true;
            }
            break;

        case SDL_CONTROLLERBUTTONUP:
            switch (event.cbutton.button)
            {
                case SDL_CONTROLLER_BUTTON_B:
                    controlBools->giveThrust = false;
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                    controlBools->isTurningRight = false;
                    break;
                case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                    controlBools->isTurningLeft = false;
                case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                    controlBools->isShooting = false;
            }
            break;
        default:
            break;
    }
}

ControlBools controlBools;
