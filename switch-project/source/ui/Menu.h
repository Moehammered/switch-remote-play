#ifndef __MENU_H__
#define __MENU_H__

#include "../SDL_FontCache.h"
#include "Text.h"
#include <vector>
#include <switch/runtime/pad.h>
#include <SDL2/SDL_render.h>

class Menu
{
    public:
        SDL_Color highlightColour;

        Menu();
        // virtual ~Menu();
        
        virtual void ProcessInput(PadState const & pad);

        virtual void Render(SDL_Renderer * const renderer, FC_Font * const font);

    protected:
        Text title;
};

#endif
