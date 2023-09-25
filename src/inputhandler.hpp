#pragma once

struct ControlBools
{
    bool giveThrust;
    bool isTurningRight;
    bool isTurningLeft;
    bool isShooting;
    bool isLeftClicking;
    bool isUsingBomb;
    bool pausePressed;
};
typedef ControlBools ControlBools;

class InputHandler
{
private:
    ControlBools controlBools;

public:
    InputHandler();
    void init();
    void handleInput(bool &isRunning);

    ControlBools getControlBools() { return controlBools; }
};
