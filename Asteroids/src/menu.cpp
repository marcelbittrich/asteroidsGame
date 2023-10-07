#include "menu.hpp"
#include "inputhandler.hpp"
#include "game.hpp"
#include "gamestate.hpp"

GameMenu::GameMenu(TTF_Font* font, TTF_Font* fontHuge, SDL_Renderer* renderer, int windowWidth, int windowHeight)
{
	this->m_width = windowWidth;
	this->m_height = windowHeight;
	this->m_font = font;
	this->m_fontHuge = fontHuge;
	this->m_renderer = renderer;

	SDL_Color textColor = { 255, 255, 255, 255 };
	SDL_Point position = {0 ,0};
	const char* text = nullptr;

	position = { m_width / 2 , 100 };
	text = "Game Over";
	createMenuText(m_gameOverTextRect, m_gameOverTextTexture, position, text, m_fontHuge, textColor, renderer);

	position = { m_width / 2 , 150 };
	text = "Asteroids";
	createMenuText(m_startScreenTextRect, m_startScreenTextTexture, position, text, m_fontHuge, textColor, renderer);

	position = { m_width / 2 , 630 };
	text = "Move - Arrows, Shoot - Space, Bomb - Ctrl";
	createMenuText(m_controlInstructionsTextRect, m_controlInstructionsTextTexture, position, text, m_font, textColor, renderer);

	m_startButtonRect = { 50, 400, m_width - 100, 80 };
	text = "Start";
	createMenuButton(m_startButtonTextRect, m_startButtonTexture, m_startButtonRect, text, font, textColor, renderer);

	m_exitButtonRect = { 50, 500, m_width - 100, 80 };
	text = "Exit";
	createMenuButton(m_exitButtonTextRect, m_exitButtonTexture, m_exitButtonRect, text, font, textColor, renderer);

	m_scoreTextRect = {};
}

void GameMenu::createMenuText(SDL_Rect& renderRect, SDL_Texture*& texture, const SDL_Point& centerPosition,
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

void GameMenu::createMenuButton(SDL_Rect& renderRect, SDL_Texture*& texture, const SDL_Rect& buttonDimensions,
	const char* text, TTF_Font* font, const SDL_Color color, SDL_Renderer* renderer)
{
	SDL_Point centerPosition;
	centerPosition.x = buttonDimensions.x + buttonDimensions.w / 2;
	centerPosition.y = buttonDimensions.y + buttonDimensions.h / 2;
	createMenuText(renderRect, texture, centerPosition, text, font, color, renderer);
}

void GameMenu::Update(bool& isRunning, GameState& gameState, const InputHandler& MyInputHandler)
{
	bool isLeftClicking = (MyInputHandler.getControlBools()).isLeftClicking;
	if (isLeftClicking)
	{
		int clickPosX, clickPosY;
		SDL_GetMouseState(&clickPosX, &clickPosY);
		relocateClick(clickPosX, clickPosY);

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

void GameMenu::relocateClick(int& clickPosX, int& clickPosY)
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
	SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);
	SDL_SetRenderDrawColor(m_renderer, 60, 60, 60, 255);

	if (m_showStartScreen)
	{
		SDL_RenderCopy(m_renderer, m_startScreenTextTexture, NULL, &m_startScreenTextRect);
	}
	else
	{
		SDL_RenderCopy(m_renderer, m_gameOverTextTexture, NULL, &m_gameOverTextRect);
	}

	SDL_RenderCopy(m_renderer, m_controlInstructionsTextTexture, NULL, &m_controlInstructionsTextRect);

	SDL_RenderFillRect(m_renderer, &m_startButtonRect);
	SDL_RenderCopy(m_renderer, m_startButtonTexture, NULL, &m_startButtonTextRect);

	SDL_RenderFillRect(m_renderer, &m_exitButtonRect);
	SDL_RenderCopy(m_renderer, m_exitButtonTexture, NULL, &m_exitButtonTextRect);

	SDL_Color color = { 255, 255, 255, 255 };
	if (!m_showStartScreen)
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
		SDL_Surface* scoreTextSurface = TTF_RenderText_Solid(m_font, scoreMessage.c_str(), color);

		m_scoreTextTexture = SDL_CreateTextureFromSurface(m_renderer, scoreTextSurface);
		SDL_FreeSurface(scoreTextSurface);
		SDL_QueryTexture(m_scoreTextTexture, NULL, NULL, &m_scoreTextRect.w, &m_scoreTextRect.h);
		m_scoreTextRect.x = m_width / 2 - m_scoreTextRect.w / 2;
		m_scoreTextRect.y = 300;
		SDL_RenderCopy(m_renderer, m_scoreTextTexture, NULL, &m_scoreTextRect);
		SDL_DestroyTexture(m_scoreTextTexture);
	}

	SDL_RenderPresent(m_renderer);
}
