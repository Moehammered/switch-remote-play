#include "Text.h"

void Text::Render(SDL_Renderer* renderer, FC_Font* font) const
{
    // SDL_Rect bg = { x, y, 500, 700 };
    // SDL_SetRenderDrawColor(renderer, colour.r ^ 255, colour.g ^ 255, colour.b ^ 255, colour.a);
    // SDL_RenderFillRect(renderer, &bg);
    FC_DrawColor(font, renderer, x, y, colour, value.c_str());
}

void Text::Render(SDL_Renderer* renderer, FC_Font* font, SDL_Color col) const
{
    FC_DrawColor(font, renderer, x, y, col, value.c_str());
}