#ifndef __MENUSELECTION_H__
#define __MENUSELECTION_H__

#include "ui/Menu.h"
#include "ui/HelpMenu.h"
#include "ui/NetworkMenu.h"
#include "ui/DecoderMenu.h"
#include "ui/EncoderMenu.h"
#include "ui/ControllerMenu.h"
#include "ui/TouchMenu.h"
#include "ui/MouseMenu.h"
#include "ui/KeyboardMenu.h"
#include "ui/AudioMenu.h"
#include "network/NetworkDiscovery.h"
#include "utils/ConfigHelper.h"
#include <string>
#include <unordered_map>

class MenuSelection : public Menu
{
    public:
        MenuSelection();

        void ProcessInput(PadState const & pad) override;

        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        void RenderTitle(SDL_Renderer * const renderer, FC_Font * const font);

        void RenderPendingConnection(SDL_Renderer * const renderer, FC_Font * const font);

        void RenderNetworkStatus(SDL_Renderer * const renderer, FC_Font * const font, NetworkDiscovery const & network);
    
        EncoderConfig const GetFfmpegSettings() const;

        DecoderData const GetDecoderSettings() const;

        controller::ControllerConfig const GetControllerSettings() const;

        mouse::MouseConfig const MouseSettings() const;

        keyboard::KeyboardConfig const KeyboardSettings() const;

        touch::TouchConfig const TouchSettings() const;

        network::NetworkConfig const NetworkSettings() const;

        ConfigContainer const ConfigurationSettings() const;

    private:
        Text controlsText;
        Text hostConnectionText;
        Text streamPendingText;
        HelpMenu helpScreen;
        EncoderMenu encoderScreen;
        NetworkMenu networkScreen;
        std::unordered_map<int32_t, Menu*> menus;

        int32_t selectedMenu;
        DecoderMenu newDecoderMenu;
        ControllerMenu newControllerMenu;
        TouchMenu touchMenu;
        MouseMenu mouseMenu;
        KeyboardMenu keyboardMenu;
        AudioMenu audioMenu;
};

#endif
