#include "gamestates.hpp"

#include <numeric>

#include "SDL.h"

#include "../game.hpp"
#include "../objects/initialization.hpp"
#include "../objects/gameobjects/asteroid.h"
#include "../objects/gameobjects/bomb.h"
#include "../objects/gameobjects/ship.h"
#include "../objects/gameobjects/shot.h"
#include "../ui/UIelements.hpp"
#include "../menu/menu.hpp"

void MenuState::Enter(Game* game)
{
	game->GetAudioPlayer().PlayMusic(MusicType::MenuMusic);
}

void MenuState::HandleEvents(Game* game, const InputHandler& inputHandler)
{
	game->GetMainMenu().HandleEvents(inputHandler);
}

void MenuState::Update(Game* game, float deltaTime)
{
	
}

void MenuState::Render(Game* game, SDL_Renderer* renderer)
{
	game->GetMainMenu().Render();
}

void MenuState::Exit(Game* game)
{
	game->GetAudioPlayer().PlaySoundEffect(EffectType::StartSound);
	game->GetAudioPlayer().PlayMusic(MusicType::GameMusic);

	game->ResetAllGameObjects();
	game->InitGameplayValues();
}

void LevelState::Enter(Game* game)
{

}

void LevelState::HandleEvents(Game* game, const InputHandler& inputHandler)
{
	for (Ship& ship : Ship::ships) 
		ship.HandleInput(inputHandler);

	game->GetCollisionHandler().CheckCollisions(game->gameObjectPtrs);

	// Destroy all dead stuff
	game->gameObjectPtrs.clear();
	// Ships
	for (auto shipIt = Ship::ships.begin(); shipIt != Ship::ships.end(); )
	{
		if (shipIt->GetIsDead())
		{
			shipIt = Ship::ships.erase(shipIt);
		}
		else
		{
			game->gameObjectPtrs.push_back(&*shipIt);
			shipIt++;
		}
	}
	// Asteroids
	for (auto asteroidIt = Asteroid::asteroids.begin(); asteroidIt != Asteroid::asteroids.end(); )
	{
		if (asteroidIt->GetIsDead())
		{
			asteroidIt = Asteroid::asteroids.erase(asteroidIt);
		}
		else
		{
			game->gameObjectPtrs.push_back(&*asteroidIt);
			asteroidIt++;
		}
	}
	// Shots
	for (auto shotIt = Shot::shots.begin(); shotIt != Shot::shots.end(); )
	{
		if (shotIt->GetIsDead())
		{
			shotIt = Shot::shots.erase(shotIt);
		}
		else
		{
			game->gameObjectPtrs.push_back(&*shotIt);
			shotIt++;
		}
	}
	// Bombs
	for (auto bombIt = Bomb::bombs.begin(); bombIt != Bomb::bombs.end(); )
	{
		bool IsDead = bombIt->GetIsDead();

		if (IsDead)
		{
			bombIt = Bomb::bombs.erase(bombIt);
		}
		else
		{
			game->gameObjectPtrs.push_back(&*bombIt);
			bombIt++;
		}
	}
	
	// Spawn New Asteroids
	if (game->GetTimeLastWave() >= ASTEROID_SPAWN_DELTATIME)
	{
		game->SpawnAsteroidWave();
	}

	bool pausePressed = inputHandler.GetControlBools().pausePressed;
	if (pausePressed && game->GetNewPausePress())
	{
		game->ChangeState(&Game::pauseState);
		game->SetNewPausePress(false);
	}
	else if (!pausePressed)
	{
		game->SetNewPausePress(true);
	}

	if (game->GetLife() == 0)
		game->ChangeState(&Game::menuState);
}

void LevelState::Update(Game* game, float deltaTime)
{
	game->AddTimeLastWave(deltaTime);

	for (GameObject* gameObject : game->GetGameObjectPtrs())
	{
		gameObject->Update(game->GetWindowDim().x, game->GetWindowDim().y, deltaTime);
	}

	game->GetBackground().Update(game->GetGameObjectPtrs(), deltaTime);

	for (UICounter& uiCounter : UICounter::UICounters)
		uiCounter.Update();
}

void LevelState::Render(Game* game, SDL_Renderer* renderer)
{
	game->GetBackground().Render(renderer);

	for (GameObject* gameObject : game->GetGameObjectPtrs())
	{
		gameObject->Render();
	}

	for (UICounter& uiCounter : UICounter::UICounters)
		uiCounter.Render();
}

void LevelState::Exit(Game* game)
{
	game->GetMainMenu().ChangeState(MainMenu::MenuState::GameOver);
	game->GetMainMenu().UpdateScore(game->GetScore());

	int oldHighscore = game->GetGameSave().GetHighscore();
	if (game->GetScore() > oldHighscore)
	{
		game->GetGameSave().SetHighscore(game->GetScore());
		game->GetGameSave().WriteFile();
	}
}

void PauseState::Enter(Game* game)
{
	game->GetAudioPlayer().PlaySoundEffect(EffectType::PauseOpen);
}

void PauseState::HandleEvents(Game* game, const InputHandler& inputHandler)
{
	game->GetPauseMenu().HandleEvents(inputHandler);

	bool pausePressed = inputHandler.GetControlBools().pausePressed;
	if (pausePressed && game->GetNewPausePress())
	{
		game->ChangeState(&Game::levelState);
		game->SetNewPausePress(false);
	}
	else if (!pausePressed)
	{
		game->SetNewPausePress(true);
	}
}

void PauseState::Update(Game* game, float deltaTime)
{
	
}

void PauseState::Render(Game* game, SDL_Renderer* renderer)
{
	game->GetBackground().Render(renderer);

	for (GameObject* gameObject : game->GetGameObjectPtrs())
	{
		gameObject->Render();
	}

	game->GetPauseMenu().Render();
}

void PauseState::Exit(Game* game)
{
	game->GetAudioPlayer().PlaySoundEffect(EffectType::PauseClose);
	game->GetGameSave().SetMasterVolume(game->GetAudioPlayer().GetMasterVolume());
	game->GetGameSave().WriteFile();
}
