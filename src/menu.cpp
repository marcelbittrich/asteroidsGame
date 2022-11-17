# include "menu.hpp"

GameMenu::GameMenu(int width, int height)
{
    this->width = width;
    this->height = height;

    menuRect = { 50, 80, width - 100, 80};
}

void GameMenu::update(GameState *state, ControlBools *controlbools)
{
    if (controlbools->isLeftClicking)
    {
        int clickPosX, clickPosY;
        SDL_GetMouseState(&clickPosX, &clickPosY);

        if (menuRect.x < clickPosX
        && menuRect.x + menuRect.w > clickPosX
        && menuRect.y < clickPosY
        && menuRect.y + menuRect.h > clickPosY)
        {
            *state = STATE_RESET;
        }  
    }
}

void GameMenu::render(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &menuRect);
    
    SDL_RenderPresent(renderer);
}
