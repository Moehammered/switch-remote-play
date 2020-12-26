#include "ManualNetworkConfig.h"
#include "Text.h"
#include <iostream>
#include "../system/Configuration.h"

auto constexpr SEGMENTS {4};
auto constexpr SaveElement {5};
auto constexpr ManualIPModeElement {6};
auto constexpr TextElementCount {6};
Text ipSegments[SEGMENTS];
Text const warningText{
    .x = 75, .y = 220,
    .colour {255, 50, 0, 255},
    .value {"Please make sure when using manual IP mode that the IP matches your PC IP.\nIf it is wrong, then you will need to close the app via the HOME button."}
};
Text const saveText {
    .x = 350, .y = 400,
    .colour {255, 255, 255, 255},
    .value {"Save IP"}
};
Text manualIPStateText {
    .x = 550, .y = 400,
    .colour {255, 255, 255, 255},
    .value {"Manual IP Disabled"}
};
int16_t ip[SEGMENTS] {192, 168, 0, 1};
int32_t selectedSegment {0};

SDL_Color constexpr highlight {200, 200, 50, 255};
auto useManualIP {false};

void LoadManualIP()
{
    auto cfg = Configuration{};
    auto manualIP = cfg.ManualIP();
    if(manualIP.size() > 0)
    {
        auto dotCounter {0};
        int16_t buffer[4]{0, 0, 0, 0};
        auto lastPoint{0};
        for(auto i = 0U; i < manualIP.size() && dotCounter < 3; ++i)
        {
            if(manualIP[i] == '.')
            {
                auto segment = manualIP.substr(lastPoint, i-lastPoint);
                // std::cout << "converting " << segment << "\n";
                buffer[dotCounter] = atoi(segment.c_str());
                ++dotCounter;
                lastPoint = i+1;
            }
        }
        if(dotCounter == 3)
        {
            auto segment = manualIP.substr(lastPoint, manualIP.size()-lastPoint);
            // std::cout << "converting " << segment << "\n";
            buffer[dotCounter] = atoi(segment.c_str());
            ++dotCounter;
            for(auto i = 0; i < dotCounter; ++i)
                ip[i] = buffer[i];
        }
        else
            std::cout << "Incorrect ip format found. Only " << dotCounter << " dots found in IP string.\n";
    }
}

void SetupManualNetworkScreen()
{
    LoadManualIP();
    SDL_Color constexpr white {255,255,255,255};
    int x = 430; int y = 350;
    int offset = 70;
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
    else if(selectedSegment == SaveElement-1)
    {
        SaveManualIP();
    }
    else if(selectedSegment == ManualIPModeElement-1)
    {
        useManualIP = !useManualIP;
        manualIPStateText.value = useManualIP ? "Manual IP Enabled" : "Manual IP Disabled";
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
    warningText.Render(renderer, systemFont);
    for(auto i = 0; i < SEGMENTS; ++i)
    {
        if(selectedSegment == i)
            ipSegments[i].Render(renderer, systemFont, highlight);
        else
            ipSegments[i].Render(renderer, systemFont);
    }
    if(selectedSegment == SaveElement-1)
        saveText.Render(renderer, systemFont, highlight);
    else
        saveText.Render(renderer, systemFont);
    if(selectedSegment == ManualIPModeElement-1)
        manualIPStateText.Render(renderer, systemFont, highlight);
    else
        manualIPStateText.Render(renderer, systemFont);
}

void SaveManualIP()
{
    auto ip = ManualIPAddress();
    auto cfg = Configuration{};
    if(!cfg.SaveManualIP(ip))
        std::cout << "Failed to manually save IP\n";
    else
        std::cout << "Saved manual IP " << ip << "\n";
}

std::string const ManualIPAddress()
{
    auto period {std::string{"."}};
    return ipSegments[0].value + period 
         + ipSegments[1].value + period
         + ipSegments[2].value + period
         + ipSegments[3].value;
}

bool ManualIPMode()
{
    return useManualIP;
}