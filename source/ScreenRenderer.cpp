#include "ScreenRenderer.h"
#include <string>
#include <iostream>
#include <switch.h>

bool ScreenRenderer::Initialise(unsigned short width, unsigned short height, bool vSync)
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
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

void ScreenRenderer::ClearScreen(SDL_Color bgCol)
{
    SDL_SetRenderDrawColor(renderer, bgCol.r, bgCol.g, bgCol.b, bgCol.a);
    SDL_RenderClear(renderer);
}

void ScreenRenderer::RenderScreenTexture()
{
    SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
    PresentScreen();
}

void ScreenRenderer::PresentScreen()
{
    SDL_RenderPresent(renderer);
}

SDL_Renderer* ScreenRenderer::Renderer()
{
    return renderer;
}

const SDL_Rect& ScreenRenderer::Region()
{
    return region;
}

SDL_Texture* ScreenRenderer::GetScreenTexture()
{
    return screenTexture;
}