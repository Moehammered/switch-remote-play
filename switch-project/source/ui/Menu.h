#ifndef __MENU_H__
#define __MENU_H__

#include <vector>
#include <switch/runtime/pad.h>
#include <SDL2/SDL_render.h>
#include "../SDL_FontCache.h"

class Menu
{
    public:
        SDL_Color highlightColour;

        Menu();
        
        virtual void ProcessInput(PadState const & pad) = 0;

        virtual void Render(SDL_Renderer * const renderer, FC_Font * const font) = 0;
};

#endif
