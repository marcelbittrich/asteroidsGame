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
    ControlBools m_controlBools;

public:
    InputHandler();
    void HandleInput(bool &isRunning);

    ControlBools GetControlBools() const { return m_controlBools; }
};
