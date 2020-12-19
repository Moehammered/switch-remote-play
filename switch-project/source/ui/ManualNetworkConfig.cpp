#include "ManualNetworkConfig.h"
#include "Text.h"
#include <iostream>

auto constexpr SEGMENTS {4};
auto constexpr TextElementCount {5};
Text ipSegments[SEGMENTS];
Text const saveText {
    .x = 100, .y = 450,
    .colour {255, 255, 255, 255},
    .value {"Save IP"}
};
int16_t ip[SEGMENTS] {192, 168, 0, 1};
int32_t selectedSegment {0};

SDL_Color constexpr highlight {200, 200, 50, 255};

void SetupManualNetworkScreen()
{
    SDL_Color constexpr white {255,255,255,255};
    int x = 100; int y = 400;
    int offset = 50;
    for(auto i = 0; i < SEGMENTS; ++i)
    {
        ipSegments[i].colour = white;
        ipSegments[i].value = std::to_string((int32_t)ip[i]);
        
        auto xPos = x + (offset * i);
        ipSegments[i].x = xPos;
        ipSegments[i].y = y;
    }
}

void UpdateText()
{
    for(auto i = 0U; i < SEGMENTS; ++i)
        ipSegments[i].value = std::to_string((int32_t)ip[i]);
}

void SelectPreviousSegment()
{
    --selectedSegment;
    if(selectedSegment < 0)
        selectedSegment = TextElementCount-1;
}

void SelectNextSegment()
{
    ++selectedSegment;
    if(selectedSegment >= TextElementCount)
        selectedSegment = 0;
}

void IncreaseSegment()
{
    if(selectedSegment < SEGMENTS)
    {
        ip[selectedSegment] += 1;
        if(ip[selectedSegment] > 255)
            ip[selectedSegment] = 0;
        UpdateText();
    }
    else if(selectedSegment == TextElementCount-1)
    {
        SaveManualIP();
    }
}

void DecreaseSegment()
{
    if(selectedSegment < SEGMENTS)
    {
        ip[selectedSegment] -= 1;
        if(ip[selectedSegment] < 0)
            ip[selectedSegment] = 255;
        UpdateText();
    }
}


void RenderNetworkConfigScreen(SDL_Renderer * const renderer, FC_Font * const systemFont)
{
    for(auto i = 0; i < SEGMENTS; ++i)
    {
        if(selectedSegment == i)
            ipSegments[i].Render(renderer, systemFont, highlight);
        else
            ipSegments[i].Render(renderer, systemFont);
    }
    if(selectedSegment == TextElementCount-1)
        saveText.Render(renderer, systemFont, highlight);
    else
        saveText.Render(renderer, systemFont);
}

void SaveManualIP()
{
    // do stuff here
}

std::string const IPAddress()
{
    auto period {std::string{"."}};
    return ipSegments[0].value + period 
         + ipSegments[1].value + period
         + ipSegments[2].value + period
         + ipSegments[3].value;
}
