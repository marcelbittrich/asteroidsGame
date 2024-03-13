#pragma once

#include "gamestate.hpp"
#include "../menu/menu.hpp"

class LevelState : public GameState
{
public:
	virtual void Enter(class Game* game) override;
	virtual void HandleEvents(const InputHandler& inputHandler) override;
	virtual void Update(float deltaTime) override;
	virtual void Render(struct SDL_Renderer* renderer) override;
	virtual void Exit() override;

private:
	class Game* m_game = nullptr;
	std::vector<class GameObject*> m_gameObjectPtrs;
	void HandlePausePressed(const InputHandler& inputHandler);
	void DestroyDeadObjects();
	void SpawnNewObjects();
	void HandleGameOver();

	void UpdateBackground(float deltaTime);
	void UpdateGameObjects(float deltaTime);
	void UpdateUI(float deltaTime);

	void RenderBackground();
	void RenderGameObjects();
	void RenderUI();

	void ChangeMainMenuState(MainMenu::MenuState state);
	void SetScoreForMenu();
	void HandleHighscore();
};
