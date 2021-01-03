#include "HelpMenu.h"

auto constexpr page1 =
"Network:\n\
- Please make sure the switch-remote-play-host application is running on your Windows PC.\n\
- Please connect to a 5GHZ wifi network if you can. (e.g. homewifi_5GHz).\n\
- Please make sure ports 19999 to 20004 aren't blocked or occupied on your PC.\n\
- Please make sure Windows Firewall isn't blocking ffmpeg or switch-remote-play-host.";

auto constexpr page2 = 
"Stream:\n\
- Hold '+' for more than 3 seconds to close the stream.\n\
- Hold 'ZL+ZR+B' or touch the screen for 3 seconds to toggle mouse/ps4 controller.\n\
- Favor lower desktop resolution to reduce latency.\n\
- You can mute your PC and audio will still stream to the switch.\
";

HelpMenu::HelpMenu() : Menu(), pages{}, buttons{}, selectedButton{1}, selectedPage{0}
{
    title.value = "Help";
    title.x += 80;

    SDL_Color constexpr orange = { 255, 190, 90, 255 };
    for(auto& page : pages)
    {
        page.x = 10; page.y = 250;
        page.colour = orange;
    }

    pages[HelpPageItems::P1].value = page1;
    pages[HelpPageItems::P2].value = page2;

    for(auto& button : buttons)
    {
        button.y = 600;
        button.colour = {255,255,255,255};
    }

    buttons[HelpPageButtons::PREV].value = "Previous Help Page";
    buttons[HelpPageButtons::PREV].x = 300;
    buttons[HelpPageButtons::NEXT].value = "Next Help Page";
    buttons[HelpPageButtons::NEXT].x = 800;
}

void HelpMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & (KEY_DDOWN | KEY_DRIGHT))
    {
        ++selectedButton;
        if(selectedButton >= HelpPageButtons::HELP_BTN_COUNT)
            selectedButton = PREV;
    }
    else if(kDown & (KEY_DUP | KEY_DLEFT))
    {
        --selectedButton;
        if(selectedButton < 0)
            selectedButton = HelpPageButtons::HELP_BTN_COUNT - 1;
    }
    else if(kDown & KEY_A)
    {
        switch(selectedButton)
        {
            case HelpPageButtons::NEXT:
                ++selectedPage;
                if(selectedPage >= HelpPageItems::HELP_PAGE_COUNT)
                    selectedPage = HelpPageItems::P1;
            break;

            case HelpPageButtons::PREV:
                --selectedPage;
                if(selectedPage < 0)
                    selectedPage = HelpPageItems::HELP_PAGE_COUNT - 1;
            break;
        }
    }
}

void HelpMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    pages[selectedPage].Render(renderer, font);

    for(auto i = 0; i < buttons.size(); ++i)
    {
        if(i == selectedButton)
            buttons[i].Render(renderer, font, highlightColour);
        else
            buttons[i].Render(renderer, font);
    }
}
