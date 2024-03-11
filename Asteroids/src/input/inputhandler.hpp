#pragma once

#include <vector>
#include "SDL.h"
#include "../vector2.hpp"

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

struct ControllerAnalogInput
{
    Vec2 LeftStick = {0,0};
    Vec2 RightStick = {0,0};
    float LeftTrigger = 0.f;
    float RightTrigger = 0.f;
};

class InputHandler
{
public:
    InputHandler();
    void HandleInput(bool& isRunning);

    void SetGameControllers(std::vector<SDL_GameController*> gameControllers) {
        m_gameControllers = gameControllers;
    }
    ControlBools GetControlBools() const { return m_controlBools; }
    ControllerAnalogInput GetControllerAnlogInput() const { return m_controllerAnalogInput; }

private:
    ControlBools m_controlBools;
    ControllerAnalogInput m_controllerAnalogInput;
    std::vector<SDL_GameController*> m_gameControllers;

    float m_stickDeadZone = 0.10f;

    float MapControllerInput(int16_t input) const;
};
