#pragma once
#include "inputhandler.hpp"

class GameState
{
public:
	virtual void Enter(class Game* game) {}
	virtual void HandleEvents(class Game* game, const InputHandler& inputHandler) {}
	virtual void Update(class Game* game, float deltaTime) {}
	virtual void Render(class Game* game, class SDL_Renderer* renderer) {}
	virtual void Exit(class Game* game) {}

protected:
	
};

class MenuState : public GameState
{
public:
	MenuState() {}

	virtual void Enter(class Game* game) override;
	virtual void HandleEvents(class Game* game, const InputHandler& inputHandler) override;
	virtual void Update(class Game* game, float deltaTime) override;
	virtual void Render(class Game* game, class SDL_Renderer* renderer) override;
	virtual void Exit(class Game* game) override;

private:
	
};

class LevelState : public GameState
{
public:
	LevelState() {}

	virtual void Enter(class Game* game) override;
	virtual void HandleEvents(class Game* game, const InputHandler& inputHandler) override;
	virtual void Update(class Game* game, float deltaTime) override;
	virtual void Render(class Game* game, class SDL_Renderer* renderer) override;
	virtual void Exit(class Game* game) override;

private:

};

class PauseState : public GameState
{
public:
	PauseState() {}

	virtual void Enter(class Game* game) override;
	virtual void HandleEvents(class Game* game, const InputHandler& inputHandler) override;
	virtual void Update(class Game* game, float deltaTime) override;
	virtual void Render(class Game* game, class SDL_Renderer* renderer) override;
	virtual void Exit(class Game* game) override;

private:

};