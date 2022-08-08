#include "ScreenRenderer.h"
#include <string>
#include <iostream>
#include <switch.h>

namespace
{
    ScreenRenderer * activeRenderer = nullptr;
}

ScreenRenderer const * MainScreenRenderer()
{
    return activeRenderer;
}

ScreenRenderer::ScreenRenderer()
{
    activeRenderer = this;
}

ScreenRenderer::~ScreenRenderer()
{
    if(activeRenderer == this)
        activeRenderer = nullptr;

    if(window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    if(renderer)
    {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if(screenTexture)
    {
        SDL_DestroyTexture(screenTexture);
        screenTexture = nullptr;
    }
}

bool ScreenRenderer::Initialise(unsigned short width, unsigned short height, bool vSync)
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::cout << "Failed to initialise SDL. SDL ERROR: " << SDL_GetError() << std::endl;
        return false;
    }

    this->height = height;
    this->width = width;
    window = SDL_CreateWindow("switch-remote-play", 0, 0, (int)width, (int)height, SDL_WINDOW_FULLSCREEN);
    if (window == nullptr)
    {
        std::cout << "Attempted to create a window of size " << (int)width << ", " << (int)height << std::endl;
        std::cout << "Error when trying to create an SDL window. SDL ERROR: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    
    Uint32 renderFlags = SDL_RENDERER_ACCELERATED;
    if(vSync)
        renderFlags |= SDL_RENDERER_PRESENTVSYNC;

    renderer = SDL_CreateRenderer(window, 0, renderFlags);
    if (renderer == nullptr)
    {
        std::cout << "Error trying to create an SDL renderer. SDL ERROR: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width, height);
    region = { 
        .x = 0, .y = 0, 
        .w = width, .h = height
    };

    return true;
}

void ScreenRenderer::ClearScreen(SDL_Color bgCol) const
{
    SDL_SetRenderDrawColor(renderer, bgCol.r, bgCol.g, bgCol.b, bgCol.a);
    SDL_RenderClear(renderer);
}

void ScreenRenderer::RenderScreenTexture()
{
    SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
    PresentScreen();
}

void ScreenRenderer::PresentScreen() const
{
    SDL_RenderPresent(renderer);
}

SDL_Renderer * const ScreenRenderer::Renderer() const
{
    return renderer;
}

SDL_Rect const & ScreenRenderer::Region() const
{
    return region;
}

SDL_Texture * const ScreenRenderer::GetScreenTexture() const
{
    return screenTexture;
}