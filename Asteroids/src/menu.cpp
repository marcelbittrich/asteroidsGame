#include "menu.hpp"
#include "inputhandler.hpp"
#include "game.hpp"
#include "gamestate.hpp"

GameMenu::GameMenu(TTF_Font* font, TTF_Font* fontHuge, SDL_Renderer* renderer, int windowWidth, int windowHeight)
	: m_width(windowWidth), m_height(windowHeight), m_font(font), m_fontHuge(fontHuge), m_renderer(renderer)
{
	SDL_Point position = {0 ,0};
	const char* text = nullptr;

	position = { m_width / 2 , 100 };
	text = "Game Over";
	CreateText(m_gameOverTextRect, m_gameOverTextTexture, position, text, m_fontHuge, m_textColor, renderer);

	position = { m_width / 2 , 150 };
	text = "Asteroids";
	CreateText(m_startScreenTextRect, m_startScreenTextTexture, position, text, m_fontHuge, m_textColor, renderer);

	position = { m_width / 2 , 630 };
	text = "Move - Arrows, Shoot - Space, Bomb - Ctrl";
	CreateText(m_controlInstructionsTextRect, m_controlInstructionsTextTexture, position, text, m_font, m_textColor, renderer);

	m_startButtonRect = { 50, 400, m_width - 100, 80 };
	text = "Start";
	CreateButton(m_startButtonTextRect, m_startButtonTexture, m_startButtonRect, text, font, m_textColor, renderer);

	m_exitButtonRect = { 50, 500, m_width - 100, 80 };
	text = "Exit";
	CreateButton(m_exitButtonTextRect, m_exitButtonTexture, m_exitButtonRect, text, font, m_textColor, renderer);

	m_scoreTextRect = {};
}

void GameMenu::CreateText(SDL_Rect& renderRect, SDL_Texture*& texture, const SDL_Point& centerPosition,
	const char* text, TTF_Font* font, const SDL_Color color, SDL_Renderer* renderer)
{
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	int textureWidth, textureHeight;
	SDL_QueryTexture(texture, NULL, NULL, &textureWidth, &textureHeight);

	renderRect.x = centerPosition.x - textureWidth / 2;
	renderRect.y = centerPosition.y - textureHeight / 2;
	renderRect.w = textureWidth;
	renderRect.h = textureHeight;
}

void GameMenu::CreateButton(SDL_Rect& renderRect, SDL_Texture*& texture, const SDL_Rect& buttonDimensions,
	const char* text, TTF_Font* font, const SDL_Color color, SDL_Renderer* renderer)
{
	SDL_Point centerPosition;
	centerPosition.x = buttonDimensions.x + buttonDimensions.w / 2;
	centerPosition.y = buttonDimensions.y + buttonDimensions.h / 2;
	CreateText(renderRect, texture, centerPosition, text, font, color, renderer);
}

void GameMenu::Update(bool& isRunning, GameState& gameState, const InputHandler& myInputHandler)
{
	bool isLeftClicking = (myInputHandler.GetControlBools()).isLeftClicking;
	if (isLeftClicking)
	{
		int clickPosX, clickPosY;
		SDL_GetMouseState(&clickPosX, &clickPosY);
		RelocateClick(clickPosX, clickPosY);

		if (m_startButtonRect.x < clickPosX
			&& m_startButtonRect.x + m_startButtonRect.w > clickPosX
			&& m_startButtonRect.y < clickPosY
			&& m_startButtonRect.y + m_startButtonRect.h > clickPosY)
		{
			gameState = GameState::STATE_RESET;
			m_showStartScreen = false;
		}

		if (m_exitButtonRect.x < clickPosX
			&& m_exitButtonRect.x + m_exitButtonRect.w > clickPosX
			&& m_exitButtonRect.y < clickPosY
			&& m_exitButtonRect.y + m_exitButtonRect.h > clickPosY)
		{
			isRunning = false;
		}
	}
}

void GameMenu::RelocateClick(int& clickPosX, int& clickPosY)
{
	int currentWidth, currentHeight;
	SDL_GetRendererOutputSize(m_renderer, &currentWidth, &currentHeight);
	int logicalWidth, logicalHeight;
	SDL_RenderGetLogicalSize(m_renderer, &logicalWidth, &logicalHeight);

	clickPosX = ((float)clickPosX / (float)currentWidth) * logicalWidth;
	clickPosY = ((float)clickPosY / (float)currentHeight) * logicalHeight;
}

void GameMenu::Render()
{
	// Clear screen to black.
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);

	// Render start or game over message
	if (m_showStartScreen)
	{
		SDL_RenderCopy(m_renderer, m_startScreenTextTexture, NULL, &m_startScreenTextRect);
	}
	else
	{
		SDL_RenderCopy(m_renderer, m_gameOverTextTexture, NULL, &m_gameOverTextRect);
	}

	SDL_RenderCopy(m_renderer, m_controlInstructionsTextTexture, NULL, &m_controlInstructionsTextRect);

	// Render buttons
	SDL_SetRenderDrawColor(m_renderer, m_buttonColor.r, m_buttonColor.g, m_buttonColor.b, m_buttonColor.a);

	SDL_RenderFillRect(m_renderer, &m_startButtonRect);
	SDL_RenderCopy(m_renderer, m_startButtonTexture, NULL, &m_startButtonTextRect);

	SDL_RenderFillRect(m_renderer, &m_exitButtonRect);
	SDL_RenderCopy(m_renderer, m_exitButtonTexture, NULL, &m_exitButtonTextRect);

	// Render score message
	if (!m_showStartScreen)
	{
		RenderScoreMessage();
	}

	SDL_RenderPresent(m_renderer);
}

void GameMenu::RenderScoreMessage()
{
	std::string scoreMessage = "";
	if (m_score == m_highscore)
	{
		scoreMessage = std::to_string(m_score) + " NEW HIGHSCORE!!!!!!!111!";
	}
	else
	{
		scoreMessage = std::to_string(m_score) + " (Highscore: " + std::to_string(m_highscore) + ") Booooo!";
	}

	SDL_Point position = { m_width / 2 , 300 };
	CreateText(m_scoreTextRect, m_scoreTextTexture, position, scoreMessage.c_str(), m_font, m_textColor, m_renderer);
	SDL_RenderCopy(m_renderer, m_scoreTextTexture, NULL, &m_scoreTextRect);
	SDL_DestroyTexture(m_scoreTextTexture);
}


