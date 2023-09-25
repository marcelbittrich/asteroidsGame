#pragma once

#include <list>

#include "SDL2/SDL.h"
#include "game.hpp"

class GameMenu
{

public:
    GameMenu(){};
    GameMenu(TTF_Font *font, TTF_Font *fontHuge, SDL_Renderer *renderer, int width, int height);
    void update(bool &isRunning, GameState &GameState, class InputHandler *MyInputHandler);
    void render();

private:
    int width, height;
    bool showStartScreen = true;
    TTF_Font *font;
    TTF_Font *fontHuge;
    SDL_Rect gameOverTextRect;
    SDL_Rect controlInstructionsTextRect;
    SDL_Rect startScreenTextRect;
    SDL_Rect startButtonRect;
    SDL_Rect startButtonTextRect;
    SDL_Rect exitButtonRect;
    SDL_Rect exitButtonTextRect;
    SDL_Rect scoreTextRect;
    SDL_Texture *gameOverTextTexture;
    SDL_Texture *controlInstructionsTextTexture;
    SDL_Texture *startScreenTextTexture;
    SDL_Texture *exitButtonTexture;
    SDL_Texture *startButtonTexture;
    SDL_Texture *scoreTextTexture;
    SDL_Renderer *renderer;

    int score;
    int highscore;

public:
    void setScore(int newScore) { score = newScore; }
    void setHighscore(int newHighscore) { highscore = newHighscore; }
};