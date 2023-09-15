#ifndef INPUTHANDLER_HPP
#define INPUTHANDLER_HPP

struct ControlBools
{
    bool giveThrust;
    bool isTurningRight;
    bool isTurningLeft;
    bool isShooting;
    bool isLeftClicking;
    bool isUsingBomb;
    bool isPaused;
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

#endif