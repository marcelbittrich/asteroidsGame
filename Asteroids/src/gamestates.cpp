#include <numeric>

#include "SDL.h"

#include "gamestates.hpp"
#include "game.hpp"
#include "gameobjects.hpp"
#include "initialization.hpp"
#include "UIelements.hpp"

void MenuState::Enter(Game* game)
{
	game->GetAudioPlayer().PlayMusic(MusicType::MenuMusic);
}

void MenuState::HandleEvents(Game* game, InputHandler& inputHandler)
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

	GameObject::ResetId();
	Ship::ships.clear();
	Asteroid::asteroids.clear();
	Shot::shots.clear();
	Bomb::bombs.clear();
	game->GetGameObjectPtrs().clear();

	game->InitGameplayValues();

	Vec2 midScreenPos = {
		game->GetWindowDim().x / 2.f,
		game->GetWindowDim().y / 2.f
	};
	Ship(midScreenPos, 50, game->shipTex);
	InitAsteroids(game->windowWidth, game->windowHeight);
}

void LevelState::Enter(Game* game)
{

}

void LevelState::HandleEvents(Game* game, InputHandler& inputHandler)
{
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

	for (Ship& ship : Ship::ships)
	{
		ship.Update(game->myInputHandler, game->GetWindowDim().x, game->GetWindowDim().y, deltaTime);
	}

	for (Asteroid& asteroid : Asteroid::asteroids)
	{
		asteroid.Update(game->GetWindowDim().x, game->GetWindowDim().y, deltaTime);
	}

	for (Shot& Shot : Shot::shots)
	{
		Shot.Update(game->GetWindowDim().x, game->GetWindowDim().y, deltaTime);
	}

	for (Bomb& bomb : Bomb::bombs)
	{
		bomb.Update(game->GetWindowDim().x, game->GetWindowDim().y, deltaTime);
	}

	game->GetBackground().Update(game->GetGameObjectPtrs(), deltaTime);

	if (deltaTime > 0)
	{
		float FPS = 1 / deltaTime;
		if (game->FPSVector.size() >= 10)
		{
			game->FPSVector.insert(game->FPSVector.begin(), FPS);
			game->FPSVector.pop_back();
		}
		else
		{
			game->FPSVector.insert(game->FPSVector.begin(), FPS);
		}
	}

	float averageFPS;
	if (!game->FPSVector.empty())
	{
		int count = (int)(game->FPSVector.size());
		averageFPS = std::reduce(game->FPSVector.begin(), game->FPSVector.end()) / (float)count;
	}
	else
	{
		averageFPS = 0;
	}

	game->UIScore.Update(game->score, game->renderer);
	game->UIFPS.Update((int)averageFPS, game->renderer);
	game->UILives.Update(game->life - 1, game->renderer);
	game->UIBomb.Update(Ship::ships[0].GetCollectedBombsSize(), game->renderer);
}

void LevelState::Render(Game* game, SDL_Renderer* renderer)
{
	game->GetBackground().Render(renderer);

	for (auto objectIt = game->GetGameObjectPtrs().begin(); objectIt != game->GetGameObjectPtrs().end(); objectIt++)
	{
		(*objectIt)->Render();
	}

	game->UIScore.Render(renderer);
	game->UILives.Render(renderer);
	game->UIBomb.Render(renderer);
	game->UIFPS.Render(renderer);
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

void PauseState::HandleEvents(Game* game, InputHandler& inputHandler)
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

	for (auto objectIt = game->gameObjectPtrs.begin(); objectIt != game->gameObjectPtrs.end(); objectIt++)
	{
		(*objectIt)->Render();
	}

	game->GetPauseMenu().Render();
}

void PauseState::Exit(Game* game)
{
	game->GetAudioPlayer().PlaySoundEffect(EffectType::PauseClose);
}
