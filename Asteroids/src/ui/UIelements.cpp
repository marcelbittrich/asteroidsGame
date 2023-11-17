#include "UIelements.hpp"

UICounter::UICounter(std::string Name, TTF_Font* m_font, SDL_Color color, SDL_Renderer* renderer, 
	int horizontalPadding, int verticalPadding, UICounterPosition counterPosition, std::function<int()> NumberGetter)
			: m_name(Name), m_font(m_font), m_color(color), m_renderer(renderer),
			m_horizontalPadding(horizontalPadding), m_verticalPadding(verticalPadding), 
			m_counterPosition(counterPosition), numberGetter(NumberGetter)
{
	// Set y position of UI: start with the default vertical padding
	m_messageRect = { 0, m_verticalPadding, 0, 0 };

	// Set y position based on how many elements are on the same side of the window.
	for (const UICounter& uiCounter : UICounters)
	{
		if (m_counterPosition == uiCounter.m_counterPosition)
			m_messageRect.y += uiCounter.m_verticalPadding * 2 + uiCounter.m_messageRect.h;
	}

	UICounters.push_back(*this);
}

void UICounter::Update()
{
	int numberToDisplay = numberGetter();
	std::string renderString = m_name + ": " + std::to_string(numberToDisplay);

	// Recreate texutre since number might have changed.
	// TODO: Could be refactored to change on events not in update.
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(m_font, renderString.c_str(), m_color);
	m_messageTexture = SDL_CreateTextureFromSurface(m_renderer, surfaceMessage);
	SDL_FreeSurface(surfaceMessage);
	SDL_QueryTexture(m_messageTexture, NULL, NULL, &m_messageRect.w, &m_messageRect.h);


	// Set x position based on UI Size 
	// For left alligned objects trivial, for right objects size is evaluated
	if (m_counterPosition == UICounterPosition::Left)
		m_messageRect.x = m_horizontalPadding;

	if (m_counterPosition == UICounterPosition::Right)
	{
		int windowWidth;
		SDL_RenderGetLogicalSize(m_renderer, &windowWidth, nullptr);
		m_messageRect.x = windowWidth - m_messageRect.w - m_horizontalPadding;
	}
}

void UICounter::Render()
{
	if (!m_renderer)
		return;

	SDL_RenderCopy(m_renderer, m_messageTexture, NULL, &m_messageRect);
	SDL_DestroyTexture(m_messageTexture);
}

ShotMeter::ShotMeter(const Ship& ship, int xOffset, int yOffset, int m_width, int m_height)
{
	this->xOffset = xOffset;
	this->yOffset = yOffset;
	position.w = m_width;
	position.h = m_height;

	Reconstruct(position, ship);
}

void ShotMeter::Reconstruct(SDL_Rect position, const Ship& ship)
{
	position.x = (int)(ship.GetMidPos().x + xOffset - position.w / 2);
	position.y = (int)(ship.GetMidPos().y + yOffset + position.h / 2);
	int borderOffset = 1;
	background1 = position;
	background2 = { background1.x + borderOffset,
				   background1.y + borderOffset,
				   background1.w - 2 * borderOffset,
				   background1.h - 2 * borderOffset };
	meterBar = background2;
}

void ShotMeter::Update(int m_shotCounter, int m_maxShotCounter, const Ship& ship)
{
	Reconstruct(position, ship);
	float shotMeterPercent = (std::min((((float)m_shotCounter / (float)m_maxShotCounter) * (float)m_maxShotCounter), (float)m_maxShotCounter)) / (float)m_maxShotCounter;
	meterBar.w = (int)(shotMeterPercent * background2.w);
}

void ShotMeter::Render(SDL_Renderer* renderer, bool m_canShoot)
{
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderFillRect(renderer, &background1);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
	SDL_RenderFillRect(renderer, &background2);
	m_canShoot ? SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255) : SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
	SDL_RenderFillRect(renderer, &meterBar);
}