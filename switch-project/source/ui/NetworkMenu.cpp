#include "NetworkMenu.h"
#include "../system/Configuration_Old.h"

#include <iostream>

NetworkMenu::NetworkMenu() : Menu(),
    warningText{}, textElements{}, ip{}, useManualIP{false}
{ 
    title.value = "Network Configuration_Old";
    warningText.x = 75; warningText.y = title.y + 60;
    warningText.colour = {255, 50, 0, 255};
    warningText.value = "Please make sure when using manual IP mode that the IP matches your PC IP.\nIf it is wrong, then you will need to close the app via the HOME button.";

    ip = LoadManualIP();

    SDL_Color constexpr white {255,255,255,255};
    int x = 430; int y = 350;
    int offset = 70;
    for(auto i = 0U; i <= NetworkMenuItems::IP_SEG_4; ++i)
    {
        textElements[i].colour = white;
        textElements[i].value = std::to_string((int32_t)ip[i]);
        
        auto xPos = x + (offset * i);
        textElements[i].x = xPos;
        textElements[i].y = y;
    }

    auto& saveButton = textElements[NetworkMenuItems::SAVE_BUTTON];
    saveButton.x = 350; saveButton.y = 400;
    saveButton.colour = white;
    saveButton.value = "Save IP";

    auto& manualToggle = textElements[NetworkMenuItems::MANUAL_TOGGLE];
    manualToggle.x = 550; manualToggle.y = 400;
    manualToggle.colour = white;

    auto cfg = Configuration_Old{};
    useManualIP = cfg.ManualIPEnabled();
    manualToggle.value = useManualIP ? "Manual IP Enabled" : "Manual IP Disabled";
}

void NetworkMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    auto selected = (int)selectedItem;
    if(kDown & KEY_DUP)
        selected--;
    else if(kDown & KEY_DDOWN)
        selected++;

    if(selected < 0)
        selected = NetworkMenuItems::NETWORK_MENU_ITEM_COUNT - 1;
    else if(selected >= NetworkMenuItems::NETWORK_MENU_ITEM_COUNT)
        selected = 0;

    selectedItem = (NetworkMenuItems)selected;

    if(kDown & KEY_A)
        ProcessIncrease();
    else if(kDown & KEY_B)
        ProcessDecrease();
}

void NetworkMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    warningText.Render(renderer, font);
    for(auto i = 0U; i < textElements.size(); ++i)
    {
        if(i == selectedItem)
            textElements[i].Render(renderer, font, highlightColour);
        else
            textElements[i].Render(renderer, font);
    }
}

bool NetworkMenu::UseManualIP() const
{
    return useManualIP;
}

std::string const NetworkMenu::ManualIPAddress() const
{
    auto period = std::string{"."};
    return textElements[NetworkMenuItems::IP_SEG_1].value + period
         + textElements[NetworkMenuItems::IP_SEG_2].value + period
         + textElements[NetworkMenuItems::IP_SEG_3].value + period
         + textElements[NetworkMenuItems::IP_SEG_4].value;
}

std::array<int16_t, 4> NetworkMenu::LoadManualIP()
{
    auto cfg = Configuration_Old{};
    auto manualIP = cfg.ManualIP();

    if(manualIP.size() > 0)
    {
        std::array<int16_t, 4> buffer{192, 168, 0, 1};
        auto dotCounter {0};
        auto lastPoint{0};
        for(auto i = 0U; i < manualIP.size() && dotCounter < 3; ++i)
        {
            if(manualIP[i] == '.')
            {
                auto segment = manualIP.substr(lastPoint, i-lastPoint);
                buffer[dotCounter] = atoi(segment.c_str());
                ++dotCounter;
                lastPoint = i+1;
            }
        }
        if(dotCounter == 3)
        {
            auto segment = manualIP.substr(lastPoint, manualIP.size()-lastPoint);
            buffer[dotCounter] = atoi(segment.c_str());
            return buffer;
        }
        else
        {
            std::cout << "Incorrect ip format found. Only " << dotCounter << " dots found in IP string.\n";
            return std::array<int16_t, 4> {192, 168, 0, 1};
        }
    }
    else
        return std::array<int16_t, 4> {192, 168, 0, 1};
}

void NetworkMenu::ProcessIncrease()
{
    switch(selectedItem)
    {
        case NetworkMenuItems::IP_SEG_1:
        case NetworkMenuItems::IP_SEG_2:
        case NetworkMenuItems::IP_SEG_3:
        case NetworkMenuItems::IP_SEG_4:
        {
            auto segment = ip[selectedItem];
            ++segment;
            if(segment > 255)
                segment = 0;
            ip[selectedItem] = segment;

            textElements[selectedItem].value = std::to_string(ip[selectedItem]);
        }
        break;

        case NetworkMenuItems::SAVE_BUTTON:
        {
            auto ip = ManualIPAddress();
            auto cfg = Configuration_Old{};
            if(!cfg.SaveManualIP(ip))
                std::cout << "Failed to manually save IP\n";
            else
                std::cout << "Saved manual IP " << ip << "\n";
            if(!cfg.SaveManualIPEnabled(useManualIP))
                std::cout << "Failed to save manual IP mode option\n";
            else
                std::cout << "Saved manual IP mode option " << useManualIP << "\n";
        }
        break;

        case NetworkMenuItems::MANUAL_TOGGLE:
        {
            useManualIP = !useManualIP;
            auto& manualToggle = textElements[NetworkMenuItems::MANUAL_TOGGLE];
            manualToggle.value = useManualIP ? "Manual IP Enabled" : "Manual IP Disabled";
        }
        break;
    }
}

void NetworkMenu::ProcessDecrease()
{
    switch(selectedItem)
    {
        case NetworkMenuItems::IP_SEG_1:
        case NetworkMenuItems::IP_SEG_2:
        case NetworkMenuItems::IP_SEG_3:
        case NetworkMenuItems::IP_SEG_4:
        {
            auto segment = ip[selectedItem];
            --segment;
            if(segment < 0)
                segment = 255;
            ip[selectedItem] = segment;

            textElements[selectedItem].value = std::to_string(ip[selectedItem]);
        }
        break;

        default:
        case NetworkMenuItems::SAVE_BUTTON:
        case NetworkMenuItems::MANUAL_TOGGLE:
        break;
    }
}