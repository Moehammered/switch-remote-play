#ifndef __SWITCHHIDDEFINITION_H__
#define __SWITCHHIDDEFINITION_H__

#ifdef __SWITCH__
extern "C"
{
    #include <switch/services/hid.h>
}
#else
#include <stdint.h>
uint32_t constexpr BIT(uint32_t const position)
{
    return 1 << position;
}
/// HidNpadButton -- taken from libnx <switch/services/hid.h> (24 Oct. 2021)
typedef enum {
    HidNpadButton_A             = BIT(0),  ///< A button / Right face button
    HidNpadButton_B             = BIT(1),  ///< B button / Down face button
    HidNpadButton_X             = BIT(2),  ///< X button / Up face button
    HidNpadButton_Y             = BIT(3),  ///< Y button / Left face button
    HidNpadButton_StickL        = BIT(4),  ///< Left Stick button
    HidNpadButton_StickR        = BIT(5),  ///< Right Stick button
    HidNpadButton_L             = BIT(6),  ///< L button
    HidNpadButton_R             = BIT(7),  ///< R button
    HidNpadButton_ZL            = BIT(8),  ///< ZL button
    HidNpadButton_ZR            = BIT(9),  ///< ZR button
    HidNpadButton_Plus          = BIT(10), ///< Plus button
    HidNpadButton_Minus         = BIT(11), ///< Minus button
    HidNpadButton_Left          = BIT(12), ///< D-Pad Left button
    HidNpadButton_Up            = BIT(13), ///< D-Pad Up button
    HidNpadButton_Right         = BIT(14), ///< D-Pad Right button
    HidNpadButton_Down          = BIT(15), ///< D-Pad Down button
    HidNpadButton_StickLLeft    = BIT(16), ///< Left Stick pseudo-button when moved Left
    HidNpadButton_StickLUp      = BIT(17), ///< Left Stick pseudo-button when moved Up
    HidNpadButton_StickLRight   = BIT(18), ///< Left Stick pseudo-button when moved Right
    HidNpadButton_StickLDown    = BIT(19), ///< Left Stick pseudo-button when moved Down
    HidNpadButton_StickRLeft    = BIT(20), ///< Right Stick pseudo-button when moved Left
    HidNpadButton_StickRUp      = BIT(21), ///< Right Stick pseudo-button when moved Up
    HidNpadButton_StickRRight   = BIT(22), ///< Right Stick pseudo-button when moved Right
    HidNpadButton_StickRDown    = BIT(23), ///< Right Stick pseudo-button when moved Left
    HidNpadButton_LeftSL        = BIT(24), ///< SL button on Left Joy-Con
    HidNpadButton_LeftSR        = BIT(25), ///< SR button on Left Joy-Con
    HidNpadButton_RightSL       = BIT(26), ///< SL button on Right Joy-Con
    HidNpadButton_RightSR       = BIT(27), ///< SR button on Right Joy-Con
    HidNpadButton_Palma         = BIT(28), ///< Top button on Poké Ball Plus (Palma) controller
    HidNpadButton_29            = BIT(29),
    HidNpadButton_HandheldLeftB = BIT(30), ///< B button on Left NES/HVC controller in Handheld mode

    HidNpadButton_AnyLeft  = HidNpadButton_Left   | HidNpadButton_StickLLeft  | HidNpadButton_StickRLeft,  ///< Bitmask containing all buttons that are considered Left (D-Pad, Sticks)
    HidNpadButton_AnyUp    = HidNpadButton_Up     | HidNpadButton_StickLUp    | HidNpadButton_StickRUp,    ///< Bitmask containing all buttons that are considered Up (D-Pad, Sticks)
    HidNpadButton_AnyRight = HidNpadButton_Right  | HidNpadButton_StickLRight | HidNpadButton_StickRRight, ///< Bitmask containing all buttons that are considered Right (D-Pad, Sticks)
    HidNpadButton_AnyDown  = HidNpadButton_Down   | HidNpadButton_StickLDown  | HidNpadButton_StickRDown,  ///< Bitmask containing all buttons that are considered Down (D-Pad, Sticks)
    HidNpadButton_AnySL    = HidNpadButton_LeftSL | HidNpadButton_RightSL,                                 ///< Bitmask containing SL buttons on both Joy-Cons (Left/Right)
    HidNpadButton_AnySR    = HidNpadButton_LeftSR | HidNpadButton_RightSR,                                 ///< Bitmask containing SR buttons on both Joy-Cons (Left/Right)
} HidNpadButton;
#endif


#endif
