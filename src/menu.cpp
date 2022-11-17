# include "menu.hpp"

GameMenu::GameMenu(TTF_Font *font, SDL_Renderer *renderer, int width, int height)
{
    this->width = width;
    this->height = height;
    this->font = font;
    this->renderer = renderer;

    startButtonRect = { 50, 80, width - 100, 80};
    SDL_Color color = { 255, 255, 255, 255 };
    startButtonTextRect = {};
    SDL_Surface *startButtonSurface = TTF_RenderText_Solid(font, "Start", color);
    startButtonTexture = SDL_CreateTextureFromSurface(renderer, startButtonSurface);
    SDL_FreeSurface(startButtonSurface);

    SDL_QueryTexture(startButtonTexture, NULL, NULL, &startButtonTextRect.w, &startButtonTextRect.h);

    startButtonTextRect.x = startButtonRect.x + startButtonRect.w / 2 - startButtonTextRect.w / 2;
    startButtonTextRect.y = startButtonRect.y + startButtonRect.h / 2 - startButtonTextRect.h / 2;

    scoreTextRect = {};
}

void GameMenu::update(GameState *state, ControlBools *controlbools)
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
        }  
    }
}

void GameMenu::render()
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &startButtonRect);

    SDL_RenderCopy(renderer, startButtonTexture, NULL, &startButtonTextRect);

    SDL_Color color = { 255, 255, 255, 255 };
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
    scoreTextRect.y = 10;
    SDL_RenderCopy(renderer, scoreTextTexture, NULL, &scoreTextRect);
    SDL_DestroyTexture(scoreTextTexture);

    SDL_RenderPresent(renderer);
}
