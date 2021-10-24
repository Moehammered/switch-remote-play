#ifndef __CONTROLLERMENU_H__
#define __CONTROLLERMENU_H__

#include "Menu.h"
#include "srp/controller/ControllerOptions.h"
#include "srp/utils/ArrayCirculator.h"
#include "srp/utils/UnorderedMapCirculator.h"

class ControllerMenu : public Menu
{
    public:
        ControllerMenu();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        controller::ControllerConfig const Settings() const;

    private:
        std::unordered_map<controller::ControllerParameters, Text> textElements;
        ArrayCirculator<controller::ControllerParameters, std::vector> paramCursor;
        UnorderedMapCirculator<controller::ControllerMode, std::string> modeCursor;
        UnorderedMapCirculator<controller::ControllerButtonMap, std::string> buttonMapCursor;
        UnorderedMapCirculator<controller::ControllerAnalogMap, std::string> leftAnalogMapCursor;
        UnorderedMapCirculator<controller::ControllerAnalogMap, std::string> rightAnalogMapCursor;
        int16_t controllerCount;
        uint32_t homeButton;
        uint32_t homeButtonTriggerTime;

        void UpdateSetting(controller::ControllerParameters, int direction);
        void UpdateUI(controller::ControllerParameters param);
        void SetupText();
};

#endif
