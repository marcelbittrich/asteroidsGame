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
    void HandleInput(bool &isRunning);

    ControlBools getControlBools() const { return controlBools; }
};
