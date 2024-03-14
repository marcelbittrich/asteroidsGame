#include "menustate.h"

#include "../game.hpp"
#include "../menu/menu.hpp"


void MenuState::Enter(Game* game)
{
	m_game = game;
	m_game->GetAudioPlayer().PlayMusic(MusicType::MenuMusic);
}

void MenuState::HandleEvents(const InputHandler& inputHandler)
{
	m_game->GetMainMenu().HandleEvents(inputHandler);

	bool pausePressed = inputHandler.GetControlBools().pausePressed;

	if (pausePressed && m_game->GetNewPausePress())
	{
		m_game->PushState(&Game::pauseState);
		m_game->SetNewPausePress(false);
	}
	else if (!pausePressed)
	{
		m_game->SetNewPausePress(true);
	}
}

void MenuState::Update(float deltaTime)
{

}

void MenuState::Render(SDL_Renderer* renderer)
{
	m_game->GetMainMenu().Render();
}

void MenuState::Exit()
{
	m_game->ResetAllGameObjects();
	m_game->SetGameplayStartValues();
}