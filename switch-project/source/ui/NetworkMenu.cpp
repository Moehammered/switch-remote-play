#include "NetworkMenu.h"
#include "../network/NetworkConfiguration.h"
#include <iostream>
#include <switch.h>

NetworkMenu::NetworkMenu() : Menu(),
    warningText{}, textElements{}, ip{},
    useManualIP{false}, selectedItem{}, broadcastIP{}
{ 
    title.value = "Network Configuration";
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
    saveButton.x = 350; saveButton.y = y+50;
    saveButton.colour = white;
    saveButton.value = "Save IP";

    auto& manualToggle = textElements[NetworkMenuItems::MANUAL_TOGGLE];
    manualToggle.x = 550; manualToggle.y = y+50;
    manualToggle.colour = white;

    auto cfg = NetworkConfiguration{};
    useManualIP = cfg.ManualIPEnabled();
    manualToggle.value = useManualIP ? "Manual IP Enabled" : "Manual IP Disabled";
  
    broadcastIP.value = cfg.BroadcastAddress();
    broadcastIP.x = x;
    broadcastIP.y = y+100;
    broadcastIP.colour = white;

    auto& testKbdIP = textElements[NetworkMenuItems::TEST_SFT_KBD_IP];
    testKbdIP.x = 350;
    testKbdIP.y = y+150;
    testKbdIP.colour = white;
    testKbdIP.value = "Set IP via Keyboard Test";
}

void NetworkMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    auto selected = (int)selectedItem;
    if(kDown & HidNpadButton_Up)
        selected--;
    else if(kDown & HidNpadButton_Down)
        selected++;

    if(selected < 0)
        selected = NetworkMenuItems::NETWORK_MENU_ITEM_COUNT - 1;
    else if(selected >= NetworkMenuItems::NETWORK_MENU_ITEM_COUNT)
        selected = 0;

    selectedItem = (NetworkMenuItems)selected;

    if(kDown & HidNpadButton_A)
        ProcessIncrease();
    else if(kDown & HidNpadButton_B)
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
    broadcastIP.Render(renderer, font);
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
    auto cfg = NetworkConfiguration{};
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
            auto cfg = NetworkConfiguration{};
            if(!cfg.SaveManualIP(ip))
                std::cout << "Failed to manually save IP\n";
            else
                std::cout << "Saved manual IP " << ip << "\n";
            if(!cfg.SaveManualEnabled(useManualIP))
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

        case NetworkMenuItems::TEST_SFT_KBD_IP:
        {
            SwkbdConfig kbd{};
            char ipEntered[16]{0};
            auto res = swkbdCreate(&kbd, 0);

            auto ipCheck = [](char* str, size_t strSize){
                auto errMsg = "Invalid IP";
                auto dotCounter = 0;
                for(auto i = 0; i < strSize; ++i)
                {
                    if(str[i] == '.')
                        ++dotCounter;
                }
                std::cout << "str size - " << strSize << '\n';
                if(dotCounter == 3)
                    return SwkbdTextCheckResult_OK;
                else
                {
                    strncpy(str, errMsg, strSize);
                    // strncpy(str, "Invalid IP", strSize);
                    return SwkbdTextCheckResult_Bad;
                }
            };

            if(R_SUCCEEDED(res))
            {
                swkbdConfigMakePresetDefault(&kbd);
                swkbdConfigSetType(&kbd, SwkbdType_NumPad);
                swkbdConfigSetLeftOptionalSymbolKey(&kbd, ".");
                swkbdConfigSetRightOptionalSymbolKey(&kbd, ".");
                swkbdConfigSetHeaderText(&kbd, "Set manual IP address");
                auto currIP = ManualIPAddress();
                swkbdConfigSetInitialText(&kbd, currIP.c_str());
                swkbdConfigSetStringLenMax(&kbd, 15);
                swkbdConfigSetTextCheckCallback(&kbd, ipCheck);

                swkbdShow(&kbd, ipEntered, 15);
                std::cout << "IP entered from kbd: " << ipEntered << "\n";
                swkbdClose(&kbd);
            }
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
        case NetworkMenuItems::TEST_SFT_KBD_IP:
        case NetworkMenuItems::SAVE_BUTTON:
        case NetworkMenuItems::MANUAL_TOGGLE:
        break;
    }
}