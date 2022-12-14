#include "SDL2/SDL.h"

#ifndef handleinput_hpp
#define handleinput_hpp

struct controlbools {
    bool giveThrust;
    bool isTurningRight;
    bool isTurningLeft;
    bool isShooting;
    bool isLeftClicking;
    bool isUsingBomb;
    bool isPaused;
};
typedef controlbools ControlBools;


void handleInput(SDL_Event event, ControlBools *controlBools, bool * isRunning);

#endif