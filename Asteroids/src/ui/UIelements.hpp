#pragma once

#include <iostream>
#include <string>
#include <list>
#include <functional>

#include "SDL.h"
#include "SDL_ttf.h"

#include "../objects/gameobjects/ship.h"

class UIElement
{
protected:
    inline static int newId = 0;
    SDL_Rect position = {0,0};
    bool m_isVisible = true;

public:
    int m_id;
    UIElement() : m_id(newId++){};
};

enum class UICounterPosition
{
    Left,
    Right
};

class UICounter : public UIElement
{

public:
    UICounter() {};
    UICounter(std::string Name, TTF_Font* m_font, SDL_Color color, SDL_Renderer* renderer,
        int horizontalPadding, int verticalPadding, UICounterPosition counterPosition, std::function<int()> numberGetter);
    void Update();
    void Render();

    // TODO: Let the Game own this
    

private:
    std::string m_name      = "Default";
    TTF_Font *m_font        = nullptr;
    SDL_Color m_color       = { 255,255,255,255 };
    int m_verticalPadding   = 0;
    int m_horizontalPadding = 0;

    UICounterPosition m_counterPosition = UICounterPosition::Left;
    std::function<int()> numberGetter = nullptr;

    SDL_Texture *m_messageTexture = nullptr; 
    SDL_Renderer* m_renderer      = nullptr;
    SDL_Rect m_messageRect        = { 0,0,0,0 };
};


// ShotMeter UIElement was an early version of the current shot meter.
// It is a bar floating under the ship, indicating how close the ship is to "overheating".
// Its currently not used in the game.
class ShotMeter : public UIElement
{
public:
    ShotMeter() {};
    ShotMeter(const Ship& ship, int xOffset, int yOffset, int m_width, int m_height);
    void Update(int m_shotCounter, int m_maxShotCounter, const Ship& ship);
    void Render(SDL_Renderer* renderer, bool m_canShoot);

private:
    SDL_Rect background1 = { 0,0 };
    SDL_Rect background2 = { 0,0 };
    SDL_Rect meterBar = { 0,0 };
    int xOffset = 0, yOffset = 0;
    void Reconstruct(SDL_Rect position, const Ship& ship);
};
