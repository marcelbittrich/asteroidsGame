# include "menu.hpp"

GameMenu::GameMenu(TTF_Font *font, TTF_Font *fontHuge, SDL_Renderer *renderer, int width, int height)
{
    this->width = width;
    this->height = height;
    this->font = font;
    this->fontHuge = fontHuge;
    this->renderer = renderer;

    SDL_Color color = { 255, 255, 255, 255 };

    SDL_Surface *gameOverTextSurface = TTF_RenderText_Solid(fontHuge, "Game Over", color);
    gameOverTextTexture = SDL_CreateTextureFromSurface(renderer, gameOverTextSurface);
    SDL_FreeSurface(gameOverTextSurface);
    SDL_QueryTexture(gameOverTextTexture, NULL, NULL, &gameOverTextRect.w, &gameOverTextRect.h);
    gameOverTextRect.x = width / 2 - gameOverTextRect.w / 2;
    gameOverTextRect.y = 100;


    SDL_Surface *startScreenTextSurface = TTF_RenderText_Solid(fontHuge, "Asteroid", color);
    startScreenTextTexture = SDL_CreateTextureFromSurface(renderer, startScreenTextSurface);
    SDL_FreeSurface(startScreenTextSurface);
    SDL_QueryTexture(startScreenTextTexture, NULL, NULL, &startScreenTextRect.w, &startScreenTextRect.h);
    startScreenTextRect.x = width / 2 - startScreenTextRect.w / 2;
    startScreenTextRect.y = 100;


    startButtonRect = { 50, 400, width - 100, 80};
    SDL_Surface *startButtonSurface = TTF_RenderText_Solid(font, "Start", color);
    startButtonTexture = SDL_CreateTextureFromSurface(renderer, startButtonSurface);
    SDL_FreeSurface(startButtonSurface);
    SDL_QueryTexture(startButtonTexture, NULL, NULL, &startButtonTextRect.w, &startButtonTextRect.h);
    startButtonTextRect.x = startButtonRect.x + startButtonRect.w / 2 - startButtonTextRect.w / 2;
    startButtonTextRect.y = startButtonRect.y + startButtonRect.h / 2 - startButtonTextRect.h / 2;

    exitButtonRect = { 50, 500, width - 100, 80};
    SDL_Surface *exitButtonSurface = TTF_RenderText_Solid(font, "Exit", color);
    exitButtonTexture = SDL_CreateTextureFromSurface(renderer, exitButtonSurface);
    SDL_FreeSurface(exitButtonSurface);
    SDL_QueryTexture(exitButtonTexture, NULL, NULL, &exitButtonTextRect.w, &exitButtonTextRect.h);
    exitButtonTextRect.x = exitButtonRect.x + exitButtonRect.w / 2 - exitButtonTextRect.w / 2;
    exitButtonTextRect.y = exitButtonRect.y + exitButtonRect.h / 2 - exitButtonTextRect.h / 2;

    scoreTextRect = {};
}

void GameMenu::update(GameState *state, ControlBools *controlbools, bool * isRunning)
{
    if (controlbools->isLeftClicking)
    {
        int clickPosX, clickPosY;
        SDL_GetMouseState(&clickPosX, &clickPosY);

        if (startButtonRect.x < clickPosX
        && startButtonRect.x + startButtonRect.w > clickPosX
        && startButtonRect.y < clickPosY
        && startButtonRect.y + startButtonRect.h > clickPosY)
        {
            *state = STATE_RESET;
            showStartScreen = false;
        }  

        if (exitButtonRect.x < clickPosX
        && exitButtonRect.x + exitButtonRect.w > clickPosX
        && exitButtonRect.y < clickPosY
        && exitButtonRect.y + exitButtonRect.h > clickPosY)
        {
            *isRunning = false;
        }  
    }
}

void GameMenu::render()
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);

    if (showStartScreen)
    {
        SDL_RenderCopy(renderer, startScreenTextTexture, NULL, &startScreenTextRect);
    } else {
        SDL_RenderCopy(renderer, gameOverTextTexture, NULL, &gameOverTextRect);
    }

    SDL_RenderFillRect(renderer, &startButtonRect);
    SDL_RenderCopy(renderer, startButtonTexture, NULL, &startButtonTextRect);

    SDL_RenderFillRect(renderer, &exitButtonRect);
    SDL_RenderCopy(renderer, exitButtonTexture, NULL, &exitButtonTextRect);

    SDL_Color color = { 255, 255, 255, 255 };
    if (!showStartScreen)
    {
        std::string scoreMessage = "";
        if (score == highscore) {
            scoreMessage = std::to_string(score) + " NEW HIGHSCORE!!!!!!!111!";
        } else {
            scoreMessage = std::to_string(score) + " (Highscore: " + std::to_string(highscore) + ") Booooo!";
        }
        SDL_Surface *scoreTextSurface = TTF_RenderText_Solid(font, scoreMessage.c_str(), color);

        scoreTextTexture = SDL_CreateTextureFromSurface(renderer, scoreTextSurface);
        SDL_FreeSurface(scoreTextSurface);
        SDL_QueryTexture(scoreTextTexture, NULL, NULL, &scoreTextRect.w, &scoreTextRect.h);
        scoreTextRect.x = width / 2 - scoreTextRect.w / 2;
        scoreTextRect.y = 300;
        SDL_RenderCopy(renderer, scoreTextTexture, NULL, &scoreTextRect);
        SDL_DestroyTexture(scoreTextTexture);
    }
    SDL_RenderPresent(renderer);
}
