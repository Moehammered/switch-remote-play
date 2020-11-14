#ifndef __TEXT_H__
#define __TEXT_H__

#include <SDL2/SDL.h>
#include <string>
#include "../SDL_FontCache.h"

class Text
{
    public:
        void Render(SDL_Renderer* renderer, FC_Font* font) const;
        void Render(SDL_Renderer* renderer, FC_Font* font, SDL_Color col) const;
        float x;
        float y;
        SDL_Color colour;
        std::string value;
};

#endif
