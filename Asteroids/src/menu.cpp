#include "menu.hpp"
#include "inputhandler.hpp"
#include "game.hpp"
#include "gamestates.hpp"
#include "audioplayer.hpp"

#include <algorithm>

GameMenu::GameMenu(TTF_Font* font, TTF_Font* fontHuge, SDL_Renderer* renderer, int windowWidth, int windowHeight, Game* owner)
	: m_width(windowWidth), m_height(windowHeight), m_font(font), m_fontHuge(fontHuge), m_renderer(renderer), m_owner(owner)
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
	SDL_Rect centeredPositionAndDimension, std::function<void()> onPressCallback, bool isVisible)
{
	TTF_Font* font = (textSize == TextSize::Small) ? m_font : m_fontHuge;
	MenuButton menuButton = CreateButton(centeredPositionAndDimension, text.c_str(), font, m_textColor, m_renderer);
	menuButton.id = id;
	menuButton.isVisible = isVisible;
	menuButton.onPressCallback = onPressCallback;
	buttonObjects.push_back(menuButton);
}

void GameMenu::AddSlider(const std::string& id, SDL_Rect centeredPositionAndDimension, std::function<void(float)> onChangeCallback, bool isVisible)
{
	Slider slider;
	slider.id = id;
	slider.isVisible = isVisible;
	slider.dimensions =
	{
		centeredPositionAndDimension.x - centeredPositionAndDimension.w / 2,
		centeredPositionAndDimension.y - centeredPositionAndDimension.h / 2,
		centeredPositionAndDimension.w,
		centeredPositionAndDimension.h
	};
	slider.indicatorDim =
	{
		slider.dimensions.x + (int)(slider.sliderValue * slider.dimensions.w) - centeredPositionAndDimension.h / 2,
		slider.dimensions.y,
		centeredPositionAndDimension.h,
		centeredPositionAndDimension.h
	};
	slider.lineDim =
	{
		slider.dimensions.x,
		slider.dimensions.y + slider.dimensions.h / 2,
		slider.dimensions.w,
		slider.lineThickness
	};
	slider.onChangeCallback = onChangeCallback;

	sliderObjects.push_back(slider);
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

void GameMenu::HandleEvents(const InputHandler& myInputHandler)
{
	SDL_Point mousePos;
	SDL_GetMouseState(&mousePos.x, &mousePos.y);
	RelocateMouse(mousePos);

	bool isLeftClicking = (myInputHandler.GetControlBools()).isLeftClicking;
	if (isLeftClicking)
	{
		for (const MenuButton& button : buttonObjects)
		{
			if (button.isVisible && IsClicked(button.buttonDim, mousePos))
			{
				if (!button.onPressCallback)
					continue;
				button.onPressCallback();
			}
		}
		for (Slider& slider : sliderObjects)
		{
			if (slider.isVisible && IsClicked(slider.indicatorDim, mousePos))
			{
				slider.isDragged = true;
			}
		}
	}
	else
	{
		for (Slider& slider : sliderObjects)
		{
			slider.isDragged = false;
		}
	}

	for (Slider& slider : sliderObjects)
	{
		if (slider.isDragged)
		{
			float newSliderValue = (mousePos.x - slider.dimensions.x) / (float)slider.dimensions.w;
			slider.sliderValue = std::clamp<float>(newSliderValue, 0.f, 1.f);

			slider.onChangeCallback(newSliderValue);

			slider.indicatorDim =
			{
				slider.dimensions.x + (int)(slider.sliderValue * slider.dimensions.w) - slider.indicatorDim.w / 2,
				slider.dimensions.y,
				slider.indicatorDim.w,
				slider.indicatorDim.h
			};
		}
	}
}

bool GameMenu::IsClicked(SDL_Rect elementDim, SDL_Point clickPos)
{
	if (elementDim.x < clickPos.x &&
		elementDim.x + elementDim.w > clickPos.x &&
		elementDim.y < clickPos.y &&
		elementDim.y + elementDim.h > clickPos.y)
		return true;
	else
		return false;
}

void GameMenu::RelocateMouse(SDL_Point& mousePos)
{
	int currentWidth, currentHeight;
	SDL_GetRendererOutputSize(m_renderer, &currentWidth, &currentHeight);
	int logicalWidth, logicalHeight;
	SDL_RenderGetLogicalSize(m_renderer, &logicalWidth, &logicalHeight);

	mousePos.x = ((float)mousePos.x / (float)currentWidth) * logicalWidth;
	mousePos.y = ((float)mousePos.y / (float)currentHeight) * logicalHeight;
}

void GameMenu::Render()
{
	for (const MenuText& text : textObjects)
	{
		if (text.isVisible)
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

	for (const Slider& slider : sliderObjects)
	{
		if (slider.isVisible)
		{
			SDL_SetRenderDrawColor(m_renderer, m_buttonColor.r, m_buttonColor.g, m_buttonColor.b, m_buttonColor.a);
			SDL_RenderFillRect(m_renderer, &slider.lineDim);
			SDL_SetRenderDrawColor(m_renderer, m_textColor.r, m_textColor.g, m_textColor.b, m_textColor.a);
			SDL_RenderFillRect(m_renderer, &slider.indicatorDim);
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
	AddButton("StartButton", "Start", TextSize::Small, posAndDim, [&]() { this->OnStartPressed(); });

	posAndDim = { m_width / 2, 500, m_width - 100, 80 };
	AddButton("ExitButton", "Exit", TextSize::Small, posAndDim, [&]() { this->OnExitPressed(); });

	position = { m_width / 2 , 630 };
	AddText("ControlsText", "Move - Arrows, Shoot - Space, Bomb - Ctrl", TextSize::Small, position);

	// GameOver
	position = { m_width / 2 , 100 };
	AddText("GameOverHeadline", "Game Over", TextSize::Large, position, false);

	position = { m_width / 2 , 300 };
	AddText("HighscoreText", "Highscore Default Message", TextSize::Small, position, false);
}

void MainMenu::ChangeState(MenuState newState)
{
	if (m_currentState != newState)
	{
		m_currentState = newState;
		OnMenuStateChange();
	}
}

void MainMenu::OnMenuStateChange()
{
	bool isStartState = (m_currentState == MenuState::Start);

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

void MainMenu::OnStartPressed()
{
	m_owner->ChangeState(&Game::levelState);
}

void MainMenu::OnExitPressed()
{
	m_owner->GetAudioPlayer().PlaySoundEffect(EffectType::EndSound);
	m_owner->ExitGame();
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

void PauseMenu::CreateDefaultPauseMenu()
{
	m_backgroundRect =
	{
		m_width / 2 - 200,
		m_height / 2 - 100,
		400,
		200
	};

	SDL_Point position = { m_width / 2, m_height / 2 - 80 };
	AddText("PauseHeadline", "PAUSE", TextSize::Small, position);

	SDL_Rect posAndDim = { m_width / 2, m_height / 2 - 30, 200, 10 };
	AddSlider("SoundSlider", posAndDim, [&](float newValue) { this->OnVolumeChange(newValue); });

	position = { m_width / 2, m_height / 2 };
	AddText("SoundText", "Sound", TextSize::Small, position);

	posAndDim = { m_width / 2, m_height / 2 + 60, 200, 40 };
	AddButton("BackButton", "Back", TextSize::Small, posAndDim, [&]() { this->OnBackPressed(); });
}

void PauseMenu::Render()
{
	SDL_SetRenderDrawColor(m_renderer, m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
	SDL_RenderFillRect(m_renderer, &m_backgroundRect);

	GameMenu::Render();
}

void PauseMenu::OnVolumeChange(float newValue)
{
	m_owner->GetAudioPlayer().SetMasterVolume(newValue);
}

void PauseMenu::OnBackPressed()
{
	m_owner->ChangeState(&Game::levelState);
}
