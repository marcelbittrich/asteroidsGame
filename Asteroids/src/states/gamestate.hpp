#pragma once

#include "../input/inputhandler.hpp"

class GameState
{
public:
	virtual void Enter(class Game* game) {}
	virtual void HandleEvents(const InputHandler& inputHandler) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Render(struct SDL_Renderer* renderer) = 0;
	virtual void Exit() = 0;
};