#pragma once

#include "SDL.h"
#include "SDL_ttf.h"
#include "gameobjects.hpp"

#include <iostream>
#include <string>
#include <list>

class UIElement
{
protected:
    static int newId;
    SDL_Rect position;
    bool m_isVisible = true;

public:
    int m_id;
    UIElement() : m_id(newId++){};
};

class ShotMeter : public UIElement
{
private:
    SDL_Rect background1;
    SDL_Rect background2;
    SDL_Rect meterBar;
    int xOffset, yOffset;
    void reconstruct(SDL_Rect position, const Ship &ship);

public:
    ShotMeter(){};
    ShotMeter(const Ship &ship, int xOffset, int yOffset, int m_width, int m_height);
    void Update(int m_shotCounter, int m_maxShotCounter, const Ship &ship);
    void Render(SDL_Renderer *renderer, bool m_canShoot);
};

enum class UICounterPosition
{
    Left,
    Right
};

class UICounter : public UIElement
{
private:
    static std::list<UICounter> UICounters;

    std::string Name;
    TTF_Font *m_font;
    SDL_Color color;
    int windowWidth, windowHeigt;
    int horizontalPadding, verticalPadding;
    UICounterPosition counterPosition;
    int numberToDisplay;
    bool displayName = false;

    SDL_Texture *messageTexture;
    SDL_Rect messageRect;

public:
    UICounter(){};
    UICounter(std::string Name, TTF_Font *m_font, SDL_Color color,
              int windowWidth, int windowHeigt,
              int horizontalPadding, int verticalPadding,
              UICounterPosition counterPosition, bool displayName);
    void Update(int numberToDisplay, SDL_Renderer *renderer);
    void Render(SDL_Renderer *renderer);
};