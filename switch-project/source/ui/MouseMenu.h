#ifndef __MOUSEMENU_H__
#define __MOUSEMENU_H__

#include "Menu.h"
#include "srp/mouse/MouseOptions.h"
#include "srp/utils/ArrayCirculator.h"
#include "srp/utils/UnorderedMapCirculator.h"

class MouseMenu : public Menu
{
public:
    MouseMenu();

    void ProcessInput(PadState const & pad) override;
    void Render(SDL_Renderer * const renderer, FC_Font * const font) override;

    mouse::MouseConfig const Settings() const;

private:
    ArrayCirculator<mouse::MouseParameters, std::vector> selectionCursor;
    std::unordered_map<mouse::MouseParameters, Text> textElements;

    UnorderedMapCirculator<HidNpadButton, std::string> leftMouseBtnCursor;
    UnorderedMapCirculator<HidNpadButton, std::string> rightMouseBtnCursor;
    UnorderedMapCirculator<HidNpadButton, std::string> middleMouseBtnCursor;
    UnorderedMapCirculator<controller::AnalogStick, std::string> mouseWheelAnalogCursor;
    bool mouseOnConnect;
    int16_t mouseSensitivity;
    uint32_t mouseToggleKey;
    uint32_t mouseToggleTime;

    bool keyAssigned(HidNpadButton const desired, std::initializer_list<HidNpadButton> const taken);
    void cycleMouseBtnCursor(UnorderedMapCirculator<HidNpadButton, std::string> & cursor, int direction, std::initializer_list<HidNpadButton> const others);
    void ChangeParam(mouse::MouseParameters param, int value);
    void UpdateUI(mouse::MouseParameters param);
    void SetupText();
};

#endif