#include "HelpMenu.h"

auto constexpr networkPage =
"- Please make sure the switch-remote-play-host application is running on your Windows PC.\n\
- Please connect to a 5GHZ wifi network if you can. (e.g. homewifi_5GHz).\n\
- Please make sure ports 19999 to 20004 aren't blocked or occupied on your PC.\n\
- Please make sure Windows Firewall isn't blocking ffmpeg or switch-remote-play-host.";

auto constexpr streamPage = 
"- Hold '+' for more than 3 seconds to close the stream.\n\
- Hold 'ZL+ZR+B' or touch the screen for 3 seconds to toggle mouse/ps4 controller.\n\
- Favor lower desktop resolution to reduce latency.\n\
- You can mute your PC and audio will still stream to the switch.\
";

HelpMenu::HelpMenu() : Menu(), pages{}, currentPageHeader{}, selectedPage{0}
{
    title.value = "Help";
    title.x += 80;

    SDL_Color constexpr orange = { 255, 190, 90, 255 };
    for(auto& page : pages)
    {
        page.x = 10; page.y = title.y + 100;
        page.colour = orange;
    }

    pages[HelpPageItems::NETWORK_PAGE].value = networkPage;
    pages[HelpPageItems::STREAM_PAGE].value = streamPage;

    currentPageHeader.y = title.y + 60;
    currentPageHeader.colour = orange;
    currentPageHeader.x = 50;

    Update();
}

void HelpMenu::ProcessInput(PadState const & pad)
{
    auto kDown = padGetButtonsDown(&pad);

    if(kDown & HidNpadButton_Right)
    {
        ++selectedPage;
        if(selectedPage >= HelpPageItems::HELP_PAGE_COUNT)
            selectedPage = HelpPageItems::NETWORK_PAGE;

        Update();
    }
    else if(kDown & HidNpadButton_Left)
    {
        --selectedPage;
        if(selectedPage < 0)
            selectedPage = HelpPageItems::HELP_PAGE_COUNT - 1;
        
        Update();
    }
}

void HelpMenu::Render(SDL_Renderer * const renderer, FC_Font * const font)
{
    title.Render(renderer, font);
    currentPageHeader.Render(renderer, font);
    pages[selectedPage].Render(renderer, font);
}

void HelpMenu::Update()
{
    switch(selectedPage)
    {
        case HelpPageItems::NETWORK_PAGE:
            currentPageHeader.value = "Network (pg 1/2) - dpad left/right";
        break;

        case HelpPageItems::STREAM_PAGE:
            currentPageHeader.value = "Stream (pg 2/2) - dpad left/right";
        break;
    }
}