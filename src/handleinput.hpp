#ifndef handleinput_hpp
#define handleinput_hpp

#include "game.hpp"

struct controlBools {
    bool giveThrust;
    bool isTurningRight;
    bool isTurningLeft;
};

typedef struct controlBools ControlBools;

void handleInput(SDL_Event event, ControlBools *controlBools, bool * isRunning);

#endif