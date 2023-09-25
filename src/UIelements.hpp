#pragma once

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "gameobjects.hpp"

#include <iostream>
#include <string>
#include <list>

class UIElement
{
protected:
    static int newId;
    SDL_Rect position;
    bool isVisible = true;

public:
    int id;
    UIElement() : id(newId++){};
};

class ShotMeter : public UIElement
{
private:
    SDL_Rect background1;
    SDL_Rect background2;
    SDL_Rect meterBar;
    int xOffset, yOffset;
    void reconstruct(SDL_Rect position, Ship *ship);

public:
    ShotMeter(){};
    ShotMeter(Ship *ship, int xOffset, int yOffset, int width, int height);
    void update(int shotCounter, int maxShotCounter, Ship *ship);
    void render(SDL_Renderer *renderer, bool canShoot);
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
    TTF_Font *font;
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
    UICounter(std::string Name, TTF_Font *font, SDL_Color color,
              int windowWidth, int windowHeigt,
              int horizontalPadding, int verticalPadding,
              UICounterPosition counterPosition, bool displayName);
    void update(int numberToDisplay, SDL_Renderer *renderer);
    void render(SDL_Renderer *renderer);
};