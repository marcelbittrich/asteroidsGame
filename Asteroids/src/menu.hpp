#pragma once

#include <list>

#include "SDL.h"
#include "SDL_ttf.h"
#include "gamestate.hpp"
#include "inputhandler.hpp"

class GameMenu
{
public:
    GameMenu() {};
    GameMenu(TTF_Font* font, TTF_Font* fontHuge, SDL_Renderer* renderer, int windowWidth, int windowHeight);
    void Update(bool& isRunning, GameState& gameState, const InputHandler& MyInputHandler);
    void Render();

    void SetScore(int newScore) { m_score = newScore; }
    void SetHighscore(int newHighscore) { m_highscore = newHighscore; }

private:
    int m_width = 0;
    int m_height = 0;
    bool m_showStartScreen = true;
    TTF_Font* m_font = nullptr;
    TTF_Font* m_fontHuge = nullptr;

    SDL_Rect m_startScreenTextRect = {0,0,0,0};
    SDL_Texture* m_startScreenTextTexture = nullptr;

    SDL_Rect m_gameOverTextRect = { 0,0,0,0 };
    SDL_Texture* m_gameOverTextTexture = nullptr;

    SDL_Rect m_scoreTextRect = { 0,0,0,0 };
    SDL_Texture* m_scoreTextTexture = nullptr;

    SDL_Rect m_startButtonRect = { 0,0,0,0 };
    SDL_Rect m_startButtonTextRect = { 0,0,0,0 };
    SDL_Texture* m_startButtonTexture = nullptr;

    SDL_Rect m_exitButtonRect = { 0,0,0,0 };
    SDL_Rect m_exitButtonTextRect = { 0,0,0,0 };
    SDL_Texture* m_exitButtonTexture = nullptr;

    SDL_Rect m_controlInstructionsTextRect = { 0,0,0,0 };
    SDL_Texture* m_controlInstructionsTextTexture = nullptr;

    SDL_Renderer* m_renderer = nullptr;

    void createMenuText(SDL_Rect& renderRect, SDL_Texture*& texture, const SDL_Point& centerPosition,
        const char* text, TTF_Font* font, const SDL_Color color, SDL_Renderer* renderer);
    void createMenuButton(SDL_Rect& renderRect, SDL_Texture*& texture, const SDL_Rect& buttonDimensions,
        const char* text, TTF_Font* font, const SDL_Color color, SDL_Renderer* renderer);

    // Relocate the click position by the current window dimensions.
    void relocateClick(int& clickPosX, int& clickPosY);

    int m_score;
    int m_highscore;
};