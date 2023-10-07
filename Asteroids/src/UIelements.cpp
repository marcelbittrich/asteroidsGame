#include "UIelements.hpp"

int UIElement::newId = 0;

ShotMeter::ShotMeter(const Ship& ship, int xOffset, int yOffset, int m_width, int m_height)
{
	this->xOffset = xOffset;
	this->yOffset = yOffset;
	position.w = m_width;
	position.h = m_height;

	reconstruct(position, ship);
}

void ShotMeter::reconstruct(SDL_Rect position, const Ship& ship)
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
	reconstruct(position, ship);
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

std::list<UICounter> UICounter::UICounters;

UICounter::UICounter(
	std::string Name,
	TTF_Font* m_font,
	SDL_Color color,
	int windowWidth,
	int windowHeigt,
	int horizontalPadding,
	int verticalPadding,
	UICounterPosition counterPosition,
	bool displayName) : Name(Name), m_font(m_font), color(color),
	windowWidth(windowWidth), windowHeigt(windowHeigt),
	horizontalPadding(horizontalPadding), verticalPadding(verticalPadding),
	counterPosition(counterPosition), displayName(displayName)
{
	messageRect = { 0, verticalPadding, 0, 0 };
	numberToDisplay = 0;

	if (!UICounters.empty())
	{
		auto it = UICounters.begin();
		while (it != UICounters.end())
		{
			if (counterPosition == it->counterPosition)
				messageRect.y += it->verticalPadding * 2 + it->messageRect.h;
			it++;
		}
	}

	UICounters.push_back(*this);
}

void UICounter::Update(int numberToDisplay, SDL_Renderer* renderer)
{
	std::string renderString = std::to_string(numberToDisplay);
	if (displayName)
		renderString = Name + ": " + renderString;

	const char* pCString = renderString.c_str();
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(m_font, pCString, color);
	messageTexture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_FreeSurface(surfaceMessage);
	SDL_QueryTexture(messageTexture, NULL, NULL, &messageRect.w, &messageRect.h);

	if (counterPosition == UICounterPosition::Left)
		messageRect.x = horizontalPadding;

	if (counterPosition == UICounterPosition::Right)
		messageRect.x = windowWidth - messageRect.w - horizontalPadding;
}

void UICounter::Render(SDL_Renderer* renderer)
{
	if (!renderer)
		return;

	SDL_RenderCopy(renderer, messageTexture, NULL, &messageRect);
	SDL_DestroyTexture(messageTexture);
}
