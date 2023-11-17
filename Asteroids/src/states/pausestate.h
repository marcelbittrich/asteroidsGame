#pragma once

#include "gamestate.hpp"

class PauseState : public GameState
{
public:
	virtual void Enter(Game* game) override;
	virtual void HandleEvents(const InputHandler& inputHandler) override;
	virtual void Update(float deltaTime) override;
	virtual void Render(struct SDL_Renderer* renderer) override;
	virtual void Exit() override;

private:
	class Game* m_game = nullptr;
};
