#ifndef __NETWORKMENU_H__
#define __NETWORKMENU_H__

#include "Menu.h"
#include "../network/NetworkOptions.h"
#include "srp/utils/ArrayCirculator.h"
#include "srp/utils/UnorderedMapCirculator.h"

class NetworkMenu : public Menu
{
    public:
        NetworkMenu();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        network::NetworkConfig const Settings() const;

    private:
        Text warningText;
        network::NetworkConfig data;

        ArrayCirculator<network::NetworkParameters, std::vector> selected;
        std::unordered_map<network::NetworkParameters, Text> textElements;

        std::string const EnterIP(std::string const defaultIP) const;
        uint16_t const EnterPort(uint16_t const defaultPort) const;
        void EditParam(network::NetworkParameters param);
        void UpdateUI(network::NetworkParameters param);
        void SetupText();
};

#endif
