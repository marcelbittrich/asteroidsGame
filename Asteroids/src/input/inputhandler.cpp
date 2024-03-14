#include "inputhandler.hpp"

#include "SDL.h"

InputHandler::InputHandler()
{
	m_controlBools = { false };
}

void InputHandler::HandleInput(bool& m_isRunning)
{
	for (SDL_GameController* controller : m_gameControllers)
	{
		m_controllerAnalogInput.LeftStick.x = MapControllerInput(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTX));
		m_controllerAnalogInput.LeftStick.y = MapControllerInput(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_LEFTY));
		m_controllerAnalogInput.RightStick.x = MapControllerInput(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTX));
		m_controllerAnalogInput.RightStick.y = MapControllerInput(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_RIGHTY));
		m_controllerAnalogInput.LeftTrigger = MapControllerInput(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT));
		m_controllerAnalogInput.RightTrigger = MapControllerInput(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT));
	}

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			m_isRunning = false;
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_UP:
			case SDLK_w:
				m_controlBools.giveThrust = true;
				break;
			case SDLK_DOWN:
			case SDLK_s:
				break;
			case SDLK_RIGHT:
			case SDLK_d:
				m_controlBools.isTurningRight = true;
				break;
			case SDLK_LEFT:
			case SDLK_a:
				m_controlBools.isTurningLeft = true;
				break;
			case SDLK_SPACE:
				m_controlBools.isShooting = true;
				break;
			case SDLK_LCTRL:
				m_controlBools.isUsingBomb = true;
				break;
			case SDLK_p:
			case SDLK_ESCAPE:
				m_controlBools.pausePressed = true;
				break;
			default:
				break;
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_UP:
			case SDLK_w:
				m_controlBools.giveThrust = false;
				break;
			case SDLK_DOWN:
			case SDLK_s:
				break;
			case SDLK_RIGHT:
			case SDLK_d:
				m_controlBools.isTurningRight = false;
				break;
			case SDLK_LEFT:
			case SDLK_a:
				m_controlBools.isTurningLeft = false;
				break;
			case SDLK_SPACE:
				m_controlBools.isShooting = false;
				break;
			case SDLK_LCTRL:
				m_controlBools.isUsingBomb = false;
				break;
			case SDLK_p:
			case SDLK_ESCAPE:
				m_controlBools.pausePressed = false;
				break;
			default:
				break;
			}
			break;

		case SDL_MOUSEBUTTONDOWN:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				m_controlBools.isLeftClickPressed = true;
				break;
			default:
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (event.button.button)
			{
			case SDL_BUTTON_LEFT:
				m_controlBools.isLeftClickPressed = false;
				break;
			default:
				break;
			}
			break;

		case SDL_CONTROLLERBUTTONDOWN:
			switch (event.cbutton.button)
			{
			case SDL_CONTROLLER_BUTTON_A:
				m_controlBools.isShooting = true;
				break;
			case SDL_CONTROLLER_BUTTON_B:
				m_controlBools.isUsingBomb = true;
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
				m_controlBools.isTurningRight = true;
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
				m_controlBools.isTurningLeft = true;
				break;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
				m_controlBools.giveThrust = true;
				break;
			case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:			
				break;
			default:
				break;
			}
			break;
		case SDL_CONTROLLERBUTTONUP:
			switch (event.cbutton.button)
			{
			case SDL_CONTROLLER_BUTTON_A:
				m_controlBools.isShooting = false;
				break;
			case SDL_CONTROLLER_BUTTON_B:
				m_controlBools.isUsingBomb = false;
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
				m_controlBools.isTurningRight = false;
				break;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
				m_controlBools.isTurningLeft = false;
				break;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
				m_controlBools.giveThrust = false;
				break;
			case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
				break;
			default:
				break;
			}
			break;

		default:
			break;
		}
	}

	m_controlBools.isLeftClicking = false;
	if(m_controlBools.isLeftClickPressed && m_newLeftClick)
	{ 
		m_newLeftClick = false;
		m_controlBools.isLeftClicking = true;
	}
	else if (!m_controlBools.isLeftClickPressed)
	{
		m_newLeftClick = true;
	}
}

float InputHandler::MapControllerInput(int16_t input) const
{
	// out in the range of 0.0 to 1.0
	float out = input / (float)std::numeric_limits<int16_t>::max();

	if (std::abs(out) <= m_stickDeadZone)
		return 0.0f;
	else if (out >= 1.0)
		return 1.0f;
	else if (out <= -1.0)
		return - 1.0f;

	return out;
}