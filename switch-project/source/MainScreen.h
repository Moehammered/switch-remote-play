#ifndef __MAINSCREEN_H__
#define __MAINSCREEN_H__

#include <string>
#include <unordered_map>
#include "ui/Menu.h"
#include "ui/HelpMenu.h"
#include "ui/NetworkMenu.h"
#include "ui/DecoderMenu.h"
#include "ui/ConfigurationScreen.h"
#include "network/NetworkDiscovery.h"

enum MenuScreen : int32_t
{
    HELP,
    DECODER_CONFIG,
    CONFIG,
    IP_SET,
    COUNT
};

SDL_Color constexpr black = {0,0,0, 255};
SDL_Color constexpr green = { 100, 200, 100, 255 };
SDL_Color constexpr orange = { 255, 190, 90, 255 };
SDL_Color constexpr red = { 200, 100, 100, 255 };
SDL_Color constexpr blue = {100, 100, 200, 255};
SDL_Color constexpr white = {255, 255, 255, 255};

class MenuSelection : public Menu
{
    public:
        MenuSelection();

        void ProcessInput(PadState const & pad) override;

        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        void RenderTitle(SDL_Renderer * const renderer, FC_Font * const font);

        void RenderPendingConnection(SDL_Renderer * const renderer, FC_Font * const font);

        void RenderNetworkStatus(SDL_Renderer * const renderer, FC_Font * const font, NetworkDiscovery const & network);
    
        FFMPEG_Config const GetFfmpegSettings();

        DecoderConfiguration const GetDecoderSettings();

        bool UseManualIP();

        std::string const GetManualIPAddress();

    private:
        Text controlsText;
        Text hostConnectionText;
        Text streamPendingText;
        HelpMenu helpScreen;
        DecoderMenu decoderScreen;
        ConfigurationScreen encoderScreen;
        NetworkMenu networkScreen;
        std::unordered_map<MenuScreen, Menu*> menus;

        MenuScreen selectedMenu;
};

#endif
