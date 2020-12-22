#ifndef __MANUALNETWORKCONFIG_H__
#define __MANUALNETWORKCONFIG_H__

#include "SDL2/SDL_render.h"
#include "../SDL_FontCache.h"
#include <string>

void SetupManualNetworkScreen();

void SelectPreviousSegment();
void SelectNextSegment();
void IncreaseSegment();
void DecreaseSegment();
void RenderNetworkConfigScreen(SDL_Renderer * const renderer, FC_Font * const systemFont);
void SaveManualIP();
std::string const ManualIPAddress();
bool ManualIPMode();

#endif
