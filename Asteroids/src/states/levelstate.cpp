#include "levelstate.h"

#include <iostream>

#include "../game.hpp"
#include "../ui/UIelements.hpp"
#include "../objects/gameobjects/asteroid.h"
#include "../objects/gameobjects/bomb.h"
#include "../objects/gameobjects/ship.h"
#include "../objects/gameobjects/shot.h"

void LevelState::Enter(Game* game)
{
	m_game = game;
}

void LevelState::HandleEvents(const InputHandler& inputHandler)
{
	HandlePausePressed(inputHandler);
	for (Ship& ship : Game::ships)
		ship.HandleInput(inputHandler);

	m_game->HandlePhysics();

	DestroyDeadObjects();
	SpawnNewObjects();
	
	if (m_game->GetLife() == 0)
		HandleGameOver();
}

void LevelState::HandlePausePressed(const InputHandler& inputHandler)
{
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

void LevelState::DestroyDeadObjects()
{
	for (auto shipIt = Game::ships.begin(); shipIt != Game::ships.end(); )
	{
		shipIt->GetIsDead() ?
			shipIt = Game::ships.erase(shipIt) :
			shipIt++;
	}

	for (auto asteroidIt = Game::asteroids.begin(); asteroidIt != Game::asteroids.end(); )
	{
		asteroidIt->GetIsDead() ?
			asteroidIt = Game::asteroids.erase(asteroidIt) :
			asteroidIt++;
	}

	for (auto shotIt = Game::shots.begin(); shotIt != Game::shots.end(); )
	{
		shotIt->GetIsDead() ?
			shotIt = Game::shots.erase(shotIt) :
			shotIt++;
	}

	for (auto bombIt = Game::bombs.begin(); bombIt != Game::bombs.end(); )
	{
		bombIt->GetIsDead() ?
			bombIt = Game::bombs.erase(bombIt) :
			bombIt++;
	}

	for (auto followerIt = Game::followers.begin(); followerIt != Game::followers.end(); )
	{
		followerIt->GetIsDead() ?
			followerIt = Game::followers.erase(followerIt) :
			followerIt++;
	}
}

void LevelState::SpawnNewObjects()
{
	if (m_game->GetTimeLastWave() >= ASTEROID_SPAWN_DELTATIME)
	{
		m_game->SpawnAsteroidWave();
	}
}

void LevelState::HandleGameOver()
{
	m_game->ChangeState(&Game::menuState);
}

void LevelState::Update(float deltaTime)
{
	m_game->AddTimeSinceLastWave(deltaTime);

	UpdateBackground(deltaTime);
	UpdateGameObjects(deltaTime);
	UpdateUI(deltaTime);
}

void LevelState::UpdateBackground(float deltaTime)
{
	m_game->GetBackground().Update(m_game->GetGameObjectPtrs(), deltaTime);
}

void LevelState::UpdateGameObjects(float deltaTime)
{
	for (Ship& ship : Game::ships)
	{
		ship.Update(deltaTime);
	}
	for (Asteroid& astereroid : Game::asteroids)
	{
		astereroid.Update(deltaTime);
	}
	for (Shot& shot : Game::shots)
	{
		shot.Update(deltaTime);
	}
	for (Bomb& bomb : Game::bombs)
	{
		bomb.Update(deltaTime);
	}
	for (Follower& follower : Game::followers)
	{
		follower.Update(deltaTime);
	}
}

void LevelState::UpdateUI(float deltaTime)
{
	for (UICounter& uiCounter : Game::UICounters)
		uiCounter.Update();
}

void LevelState::Render(SDL_Renderer* renderer)
{
	RenderBackground();

	RenderGameObjects();

	RenderUI();
}

void LevelState::RenderBackground()
{
	m_game->GetBackground().Render();
}

void LevelState::RenderGameObjects()
{
	for (Ship& ship : Game::ships)
	{
		ship.Render();
	}
	for (Follower& follower : Game::followers)
	{
		follower.Render();
	}
	for (Asteroid& astereroid : Game::asteroids)
	{
		astereroid.Render();
	}
	for (Shot& shot : Game::shots)
	{
		shot.Render();
	}
	for (Bomb& bomb : Game::bombs)
	{
		bomb.Render();
	}
}

void LevelState::RenderUI()
{
	for (UICounter& uiCounter : Game::UICounters)
		uiCounter.Render();
}

void LevelState::Exit()
{
	ChangeMainMenuState(MainMenu::MenuState::GameOver);
	SetScoreForMenu();
	HandleHighscore();

	m_game = nullptr;
}


void LevelState::ChangeMainMenuState(MainMenu::MenuState state)
{
	m_game->GetMainMenu().ChangeState(MainMenu::MenuState::GameOver);
}

void LevelState::SetScoreForMenu()
{
	m_game->GetMainMenu().UpdateScore(m_game->GetScore());
}

void LevelState::HandleHighscore()
{
	int oldHighscore = m_game->GetGameSave().GetHighscore();
	if (m_game->GetScore() > oldHighscore)
	{
		m_game->GetGameSave().SetHighscore(m_game->GetScore());
		m_game->GetGameSave().WriteFile();
	}
}