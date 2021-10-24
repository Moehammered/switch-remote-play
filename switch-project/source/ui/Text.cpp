#include "Text.h"
#include "srp/utils/StringUtils.h"
#include <algorithm>

void Text::Render(SDL_Renderer* renderer, FC_Font* font) const
{
    // SDL_Rect bg = { x, y, 500, 700 };
    // SDL_SetRenderDrawColor(renderer, colour.r ^ 255, colour.g ^ 255, colour.b ^ 255, colour.a);
    // SDL_RenderFillRect(renderer, &bg);
    if(centered)
    {
        auto lines = stringutil::splitLines(value);
        auto maxLength = std::max_element(lines.begin(), lines.end(), [](std::string const& a, std::string const& b){
            return a.length() > b.length();
        });
        if(maxLength != lines.end())
        {
            auto lengthHalf = maxLength->length()/2;
            auto offset = 5 * lengthHalf;

            FC_DrawColor(font, renderer, x-offset, y, colour, value.c_str());
        }
        else
            FC_DrawColor(font, renderer, x, y, colour, value.c_str());
    }
    else
        FC_DrawColor(font, renderer, x, y, colour, value.c_str());
}

void Text::Render(SDL_Renderer* renderer, FC_Font* font, SDL_Color col) const
{
    FC_DrawColor(font, renderer, x, y, col, value.c_str());
}