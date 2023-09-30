#include "inputhandler.hpp"
#include "SDL2/SDL.h"

InputHandler::InputHandler()
{
    controlBools = {false};
}

void InputHandler::handleInput(bool &isRunning)
{

    // std::cout << "handle keyboard input: " << event.key.keysym.sym << std::endl;
    // std::cout << "controller input" << event.type << std::endl;

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
                controlBools.giveThrust = true;
                break;
            case SDLK_DOWN:
                break;
            case SDLK_RIGHT:
                controlBools.isTurningRight = true;
                break;
            case SDLK_LEFT:
                controlBools.isTurningLeft = true;
                break;
            case SDLK_SPACE:
                controlBools.isShooting = true;
                break;
            case SDLK_LCTRL:
                controlBools.isUsingBomb = true;
                break;
            case SDLK_p:
                controlBools.pausePressed = true;
                break;
            default:
                break;
            }
            break;
        case SDL_KEYUP:
            switch (event.key.keysym.sym)
            {
            case SDLK_UP:
                controlBools.giveThrust = false;
                break;
            case SDLK_DOWN:
                break;
            case SDLK_RIGHT:
                controlBools.isTurningRight = false;
                break;
            case SDLK_LEFT:
                controlBools.isTurningLeft = false;
                break;
            case SDLK_SPACE:
                controlBools.isShooting = false;
                break;
            case SDLK_LCTRL:
                controlBools.isUsingBomb = false;
                break;
            case SDLK_p:
                controlBools.pausePressed = false;
                break;
            default:
                break;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                controlBools.isLeftClicking = true;
                break;
            default:
                break;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            switch (event.button.button)
            {
            case SDL_BUTTON_LEFT:
                controlBools.isLeftClicking = false;
                break;
            default:
                break;
            }
            break;

        case SDL_CONTROLLERBUTTONDOWN:
            switch (event.cbutton.button)
            {
            case SDL_CONTROLLER_BUTTON_B:
                controlBools.giveThrust = true;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                controlBools.isTurningRight = true;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                controlBools.isTurningLeft = true;
                break;
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                controlBools.isShooting = true;
                break;
            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                controlBools.isUsingBomb = true;
                break;
            default:
                break;
            }
            break;
        case SDL_CONTROLLERBUTTONUP:
            switch (event.cbutton.button)
            {
            case SDL_CONTROLLER_BUTTON_B:
                controlBools.giveThrust = false;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
                controlBools.isTurningRight = false;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
                controlBools.isTurningLeft = false;
                break;
            case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
                controlBools.isShooting = false;
                break;
            case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
                controlBools.isUsingBomb = false;
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }
    }
}
