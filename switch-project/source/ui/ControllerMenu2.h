#ifndef __CONTROLLERMENU2_H__
#define __CONTROLLERMENU2_H__

#include "Menu.h"
#include "../controller/ControllerOptions.h"
#include "../utils/ArrayCirculator.h"
#include "../utils/UnorderedMapCirculator.h"
#include "../utils/RangedIterator.h"

class ControllerMenu2 : public Menu
{
    public:
        ControllerMenu2();

        void ProcessInput(PadState const & pad) override;
        void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

        controller::ControllerConfig const Settings() const;

    private:
        std::unordered_map<controller::Parameters, Text> textElements;
        ArrayCirculator<controller::Parameters, std::vector> paramCursor;
        UnorderedMapCirculator<controller::ControllerMode, std::string> modeCursor;
        UnorderedMapCirculator<controller::ControllerButtonMap, std::string> buttonMapCursor;
        UnorderedMapCirculator<controller::ControllerAnalogMap, std::string> leftAnalogMapCursor;
        UnorderedMapCirculator<controller::ControllerAnalogMap, std::string> rightAnalogMapCursor;
        UnorderedMapCirculator<HidNpadButton, std::string> leftMouseCursor;
        UnorderedMapCirculator<HidNpadButton, std::string> rightMouseCursor;
        NumberRange<int32_t, controller::MinMouseSensitivity, controller::MaxMouseSensitity> mouseSensitivity;
        bool mouseOnConnect;

        void UpdateSetting(controller::Parameters, int direction);
        void UpdateUI(controller::Parameters param);
        void SetupText();
};

#endif
