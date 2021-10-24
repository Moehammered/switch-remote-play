#ifndef __SCREENRENDERER__H_
#define __SCREENRENDERER__H_

#include "SDL_FontCache.h"
#include <SDL2/SDL.h>
#include <string>

class ScreenRenderer
{
    public:
        ScreenRenderer();
        ~ScreenRenderer();
        bool Initialise(unsigned short width, unsigned short height, bool vSync);
        void ClearScreen(SDL_Color clearColour) const;
        void RenderScreenTexture();
        void PresentScreen() const;
        
        SDL_Renderer * const Renderer() const;
        SDL_Texture * const GetScreenTexture() const;
        SDL_Rect const & Region() const;

    private:
        unsigned short width, height;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* screenTexture;
        SDL_Rect region;
};

ScreenRenderer const * MainScreenRenderer();

#endif