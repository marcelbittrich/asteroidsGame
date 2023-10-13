#pragma once

#include <vector>
#include <string>

#include "SDL.h"
#include "SDL_ttf.h"
#include "gamestate.hpp"
#include "inputhandler.hpp"


struct MenuText
{
	std::string id			= "Default";
	SDL_Rect textDim		= { 0,0,0,0 };
	SDL_Texture* texture	= nullptr;
	bool isVisible			= true;
};

struct MenuButton : MenuText
{
	SDL_Rect buttonDim		= { 0,0,0,0 };
	void (*callback)()		= nullptr;
};

class GameMenu
{
public:
	GameMenu() {};
	GameMenu(TTF_Font* font, TTF_Font* fontHuge, SDL_Renderer* renderer, int windowWidth, int windowHeight);
	~GameMenu();
	void Update(const InputHandler& myInputHandler);
	virtual void Render();

	enum class TextSize 
	{
		Small,
		Large
	};

	void AddText(const std::string& id, const std::string& text, TextSize textSize, 
		SDL_Point centeredPosition, bool isVisible = true);
	void AddButton(const std::string& id, const std::string& text, TextSize textSize, 
		SDL_Rect centeredPositionAndDimension, void(*callback)(), bool isVisible = true);

protected:
	int m_width = 0;
	int m_height = 0;

	std::vector<MenuText> textObjects = {};
	std::vector<MenuButton> buttonObjects = {};

	MenuText CreateText(const SDL_Point& centerPosition, const char* text, TTF_Font* font,
		const SDL_Color color, SDL_Renderer* renderer);
	MenuButton CreateButton(const SDL_Rect& centeredPositionAndDimension, const char* text, TTF_Font* font,
		const SDL_Color color, SDL_Renderer* renderer);

	SDL_Color m_textColor = { 255, 255, 255, 255 };
	SDL_Color m_buttonColor = { 60, 60, 60, 255 };

	TTF_Font* m_font = nullptr;
	TTF_Font* m_fontHuge = nullptr;

	SDL_Renderer* m_renderer = nullptr;

private:
	bool IsButtonClicked(MenuButton button, SDL_Point clickPos);
	// Relocate the click position by the current window dimensions.
	void RelocateClick(SDL_Point& clickPos);
};

class MainMenu : public GameMenu 
{
public:
	enum class State 
	{
		Start,
		GameOver
	};

	MainMenu() {};
	MainMenu(TTF_Font* font, TTF_Font* fontHuge, SDL_Renderer* renderer, int windowWidth, int windowHeight) 
		: GameMenu(font, fontHuge, renderer, windowWidth, windowHeight) {};
	void CreateDefaultMainMenu();

	void ChangeState(State newState);
	void UpdateScore(int newScore);

private:
	State m_currentState = State::Start;
	bool m_showStartScreen = true;

	int m_highscore = 0;
	void OnStateChange();
};

class PauseMenu : public GameMenu
{
public:
	PauseMenu() {};
	PauseMenu(TTF_Font* font, TTF_Font* fontHuge, SDL_Renderer* renderer, int windowWidth, int windowHeight)
		: GameMenu(font, fontHuge, renderer, windowWidth, windowHeight) {};
	
	void CreateDefaultPauseMenu();

	void Render() override;

private:
	SDL_Color m_backgroundColor = { 30,30,30,255 };
	SDL_Rect m_backgroundRect = { 0,0,0,0 };
}; 