#ifndef __SCREENRENDERER__H_
#define __SCREENRENDERER__H_

#include <SDL2/SDL.h>

class ScreenRenderer
{
    public:
        bool Initialise(unsigned short width, unsigned short height);
        void ClearScreen(SDL_Color clearColour);
        void RenderScreenTexture();
        void PresentScreen();
        SDL_Texture* GetScreenTexture();
        SDL_Rect GetRegion();

    private:
        unsigned short width, height;
        SDL_Window* window;
        SDL_Renderer* renderer;
        SDL_Texture* screenTexture;
        SDL_Rect region;
};

#endif