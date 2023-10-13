#include "menu.hpp"
#include "inputhandler.hpp"
#include "game.hpp"
#include "gamestate.hpp"

GameMenu::GameMenu(TTF_Font* font, TTF_Font* fontHuge, SDL_Renderer* renderer, int windowWidth, int windowHeight)
	: m_width(windowWidth), m_height(windowHeight), m_font(font), m_fontHuge(fontHuge), m_renderer(renderer)
{}

GameMenu::~GameMenu()
{
	for (MenuText& textObject : textObjects)
	{
		SDL_DestroyTexture(textObject.texture);
	}
	for (MenuButton& buttonObject : buttonObjects)
	{
		SDL_DestroyTexture(buttonObject.texture);
	}
}

void GameMenu::AddText(const std::string& id, const std::string& text, TextSize textSize, 
	SDL_Point centeredPosition, bool isVisible)
{
	TTF_Font* font = (textSize == TextSize::Small) ? m_font : m_fontHuge;
	MenuText menuText = CreateText(centeredPosition, text.c_str(), font, m_textColor, m_renderer);
	menuText.id = id;
	menuText.isVisible = isVisible;
	textObjects.push_back(menuText);
}

void GameMenu::AddButton(const std::string& id, const std::string& text, TextSize textSize, 
	SDL_Rect centeredPositionAndDimension, void(*callback)(), bool isVisible)
{	
	TTF_Font* font = (textSize == TextSize::Small) ? m_font : m_fontHuge;
	MenuButton menuButton = CreateButton(centeredPositionAndDimension, text.c_str(), font, m_textColor, m_renderer);
	menuButton.id = id;
	menuButton.isVisible = isVisible;
	menuButton.callback = callback;
	buttonObjects.push_back(menuButton);
}

MenuText GameMenu::CreateText(const SDL_Point& centerPosition,
	const char* text, TTF_Font* font, const SDL_Color color, SDL_Renderer* renderer)
{
	MenuText Text;
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	Text.texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	int textureWidth, textureHeight;
	SDL_QueryTexture(Text.texture, NULL, NULL, &textureWidth, &textureHeight);

	Text.textDim =
	{
		centerPosition.x - textureWidth / 2,
		centerPosition.y - textureHeight / 2,
		textureWidth,
		textureHeight
	};

	return Text;
}

MenuButton GameMenu::CreateButton(const SDL_Rect& centeredPositionAndDimension,
	const char* text, TTF_Font* font, const SDL_Color color, SDL_Renderer* renderer)
{
	SDL_Point centerPosition = 
	{
		centeredPositionAndDimension.x,
		centeredPositionAndDimension.y
	};
	
	MenuText buttonText = CreateText(centerPosition, text, font, color, renderer);
	MenuButton button;
	button.textDim = buttonText.textDim;
	button.texture = buttonText.texture;
	button.buttonDim =
	{
		centerPosition.x - centeredPositionAndDimension.w / 2,
		centerPosition.y - centeredPositionAndDimension.h / 2,
		centeredPositionAndDimension.w,
		centeredPositionAndDimension.h
	};

	return button;
}

void GameMenu::Update(const InputHandler& myInputHandler)
{
	bool isLeftClicking = (myInputHandler.GetControlBools()).isLeftClicking;
	if (isLeftClicking)
	{
		SDL_Point clickPos;
		SDL_GetMouseState(&clickPos.x, &clickPos.y);
		RelocateClick(clickPos);

		for (const MenuButton& button : buttonObjects)
		{
			if (button.isVisible && IsButtonClicked(button, clickPos))
			{
				if (!button.callback)
					continue;
				button.callback();
			}
		}
	}
}

bool GameMenu::IsButtonClicked(MenuButton button, SDL_Point clickPos)
{
	if (button.buttonDim.x < clickPos.x &&
		button.buttonDim.x + button.buttonDim.w > clickPos.x &&
		button.buttonDim.y < clickPos.y &&
		button.buttonDim.y + button.buttonDim.h > clickPos.y)
		return true;
	else
		return false;
}

void GameMenu::RelocateClick(SDL_Point& clickPos)
{
	int currentWidth, currentHeight;
	SDL_GetRendererOutputSize(m_renderer, &currentWidth, &currentHeight);
	int logicalWidth, logicalHeight;
	SDL_RenderGetLogicalSize(m_renderer, &logicalWidth, &logicalHeight);

	clickPos.x = ((float)clickPos.x / (float)currentWidth) * logicalWidth;
	clickPos.y = ((float)clickPos.y / (float)currentHeight) * logicalHeight;
}

void GameMenu::Render()
{
	// Clear screen to black.
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);

	for (const MenuText& text : textObjects)
	{
		if(text.isVisible)
			SDL_RenderCopy(m_renderer, text.texture, NULL, &text.textDim);
	}

	for (const MenuButton& button : buttonObjects)
	{
		if (button.isVisible)
		{
			SDL_SetRenderDrawColor(m_renderer, m_buttonColor.r, m_buttonColor.g, m_buttonColor.b, m_buttonColor.a);
			SDL_RenderFillRect(m_renderer, &button.buttonDim);
			SDL_RenderCopy(m_renderer, button.texture, NULL, &button.textDim);
		}
			
	}

	SDL_RenderPresent(m_renderer);
}

void MainMenu::CreateDefaultMainMenu()
{
	// Start
	SDL_Point position = { m_width / 2 , 150 };
	AddText("AsteroidsHeadline", "Asteroids", TextSize::Large, position);

	SDL_Rect posAndDim = { m_width / 2, 400, m_width - 100, 80 };
	AddButton("StartButton", "Start", TextSize::Small, posAndDim, Game::changeStateToReset);

	posAndDim = { m_width / 2, 500, m_width - 100, 80 };
	AddButton("ExitButton", "Exit", TextSize::Small, posAndDim, Game::exitGame);

	position = { m_width / 2 , 630 };
	AddText("ControlsText", "Move - Arrows, Shoot - Space, Bomb - Ctrl", TextSize::Small, position);

	// GameOver
	position = { m_width / 2 , 100 };
	AddText("GameOverHeadline", "Game Over", TextSize::Large, position, false);

	position = { m_width / 2 , 300 };
	AddText("HighscoreText", "Highscore Default Message", TextSize::Small, position, false);
}

void MainMenu::ChangeState(State newState)
{
	if (m_currentState != newState) 
	{
		m_currentState = newState;
		OnStateChange();
	}
}

void MainMenu::OnStateChange()
{
	bool isStartState = (m_currentState == State::Start);

	for (MenuText& text : textObjects)
	{
		if (text.id == "AsteroidsHeadline")
			text.isVisible = isStartState;
		if (text.id == "GameOverHeadline")
			text.isVisible = !isStartState;
		if (text.id == "HighscoreText")
			text.isVisible = !isStartState;
	}
}

void MainMenu::UpdateScore(int newScore)
{
	std::string scoreMessage;

	if (newScore > m_highscore)
	{
		scoreMessage = std::to_string(newScore) + " NEW HIGHSCORE!!!!!!!111!";
		m_highscore = newScore;
	}
	else
	{
		scoreMessage = std::to_string(newScore) + " (Highscore: " + std::to_string(m_highscore) + ") Booooo!";
	}

	for (MenuText& text : textObjects)
	{
		if (text.id == "HighscoreText")
		{
			SDL_Point centerPosition =
			{
				text.textDim.x + text.textDim.w / 2,
				text.textDim.y + text.textDim.h / 2
			};

			//TODO: refactor into chanfe text function and text property
			MenuText newText = CreateText(centerPosition, scoreMessage.c_str(), m_font, m_textColor, m_renderer);
			SDL_DestroyTexture(text.texture);
			text.texture = newText.texture;
			text.textDim = newText.textDim;
		}
	}
}

