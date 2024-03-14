#include "pausestate.h"

#include "../game.hpp"

void PauseState::Enter(Game* game)
{
	m_game = game;
	m_game->GetAudioPlayer().PlaySoundEffect(EffectType::PauseOpen);
}

void PauseState::HandleEvents(const InputHandler& inputHandler)
{
	m_game->GetPauseMenu().HandleEvents(inputHandler);

	bool pausePressed = inputHandler.GetControlBools().pausePressed;
	if (pausePressed && m_game->GetNewPausePress())
	{
		m_game->PopState();
		m_game->SetNewPausePress(false);
	}
	else if (!pausePressed)
	{
		m_game->SetNewPausePress(true);
	}
}

void PauseState::Update(float deltaTime)
{

}

void PauseState::Render(SDL_Renderer* renderer)
{
	m_game->GetPauseMenu().Render();
}

void PauseState::Exit()
{
	m_game->GetAudioPlayer().PlaySoundEffect(EffectType::PauseClose);
	m_game->GetGameSave().SetMasterVolume(m_game->GetAudioPlayer().GetMasterVolume());
	m_game->GetGameSave().SetEffectVolume(m_game->GetAudioPlayer().GetEffectVolume());
	m_game->GetGameSave().SetMusicVolume(m_game->GetAudioPlayer().GetMusicVolume());
	m_game->GetGameSave().WriteFile();
	m_game = nullptr;
}