#pragma once

#include <vector>
#include <string>
#include <functional>

#include "SDL.h"
#include "SDL_ttf.h"
#include "gamestates.hpp"
#include "inputhandler.hpp"
#include "audioplayer.hpp"


static struct MenuText
{
	std::string id			= "Default";
	SDL_Rect textDim		= { 0,0,0,0 };
	SDL_Texture* texture	= nullptr;
	bool isVisible			= true;
};

static struct MenuButton : MenuText
{
	SDL_Rect buttonDim		= { 0,0,0,0 };
	std::function<void()> onPressCallback;
};

static struct Slider
{
	std::string id			= "DefaultSlider";
	SDL_Rect dimensions		= { 0,0,0,0 };
	SDL_Rect indicatorDim	= { 0,0,0,0 };
	SDL_Rect lineDim		= { 0,0,0,0 };
	int lineThickness		= 2;
	float sliderValue		= 0.5;
	bool isDragged			= false;
	bool isVisible			= true;
	std::function<void(float)> onChangeCallback;
};

class GameMenu
{
public:
	GameMenu() {};
	GameMenu(TTF_Font* font, TTF_Font* fontHuge, SDL_Renderer* renderer, int windowWidth, int windowHeight,class Game* owner);
	~GameMenu();
	void HandleEvents(const InputHandler& myInputHandler);
	virtual void Render();

	enum class TextSize 
	{
		Small,
		Large
	};

	void AddText(const std::string& id, const std::string& text, TextSize textSize, 
		SDL_Point centeredPosition, bool isVisible = true);
	void AddButton(const std::string& id, const std::string& text, TextSize textSize, 
		SDL_Rect centeredPositionAndDimension, std::function<void()> onPressCallback, bool isVisible = true);
	void AddSlider(const std::string& id, SDL_Rect centeredPositionAndDimension, std::function<void(float)> onChangeCallback, bool isVisible = true);

protected:
	int m_width = 0;
	int m_height = 0;

	std::vector<MenuText> textObjects = {};
	std::vector<MenuButton> buttonObjects = {};
	std::vector<Slider> sliderObjects = {};

	MenuText CreateText(const SDL_Point& centerPosition, const char* text, TTF_Font* font,
		const SDL_Color color, SDL_Renderer* renderer);
	MenuButton CreateButton(const SDL_Rect& centeredPositionAndDimension, const char* text, TTF_Font* font,
		const SDL_Color color, SDL_Renderer* renderer);

	SDL_Color m_textColor = { 255, 255, 255, 255 };
	SDL_Color m_buttonColor = { 60, 60, 60, 255 };

	TTF_Font* m_font = nullptr;
	TTF_Font* m_fontHuge = nullptr;

	SDL_Renderer* m_renderer = nullptr;
	class Game* m_owner = nullptr;

private:
	bool IsClicked(SDL_Rect elementDim, SDL_Point clickPos);
	// Relocate the mouse position to the logical window size by the current window dimensions.
	void RelocateMouse(SDL_Point& mousePos);
};

class MainMenu : public GameMenu 
{
public:
	enum class MenuState 
	{
		Start,
		GameOver
	};

	MainMenu() {};
	MainMenu(TTF_Font* font, TTF_Font* fontHuge, SDL_Renderer* renderer, int windowWidth, int windowHeight,class Game* owner) 
		: GameMenu(font, fontHuge, renderer, windowWidth, windowHeight, owner) {};
	void CreateDefaultMainMenu();

	void ChangeState(MenuState newState);
	void UpdateScore(int newScore);

private:
	MenuState m_currentState = MenuState::Start;
	bool m_showStartScreen = true;

	int m_highscore = 0;
	void OnMenuStateChange();
	void OnStartPressed();
	void OnExitPressed();
};

class PauseMenu : public GameMenu
{
public:
	PauseMenu() {};
	PauseMenu(TTF_Font* font, TTF_Font* fontHuge, SDL_Renderer* renderer, int windowWidth, int windowHeight, class Game* owner)
		: GameMenu(font, fontHuge, renderer, windowWidth, windowHeight, owner) {};
	
	void CreateDefaultPauseMenu();

	void Render() override;

private:
	SDL_Color	m_backgroundColor	= { 30,30,30,255 };
	SDL_Rect	m_backgroundRect	= { 0,0,0,0 };

	void OnVolumeChange(float);
	void OnBackPressed();
}; 