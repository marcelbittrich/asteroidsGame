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
    SDL_RenderPresent(renderer);
}
