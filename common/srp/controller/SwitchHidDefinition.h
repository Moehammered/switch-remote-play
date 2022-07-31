#ifndef __SWITCHHIDDEFINITION_H__
#define __SWITCHHIDDEFINITION_H__

#ifdef __SWITCH__
extern "C"
{
    #include <switch/services/hid.h>
}
#else
#include <stdint.h>

typedef struct HidVector {
    float x;
    float y;
    float z;
} HidVector;

uint32_t constexpr BIT(uint32_t const position)
{
    return 1U << position;
}

uint64_t constexpr BITL(uint32_t const position)
{
    return 1ULL << position;
}

// HidNpadButton -- taken from libnx <switch/services/hid.h> (29 Jul. 2022)
typedef enum : uint64_t {
    HidNpadButton_A             = BITL(0),  // A button / Right face button
    HidNpadButton_B             = BITL(1),  // B button / Down face button
    HidNpadButton_X             = BITL(2),  // X button / Up face button
    HidNpadButton_Y             = BITL(3),  // Y button / Left face button
    HidNpadButton_StickL        = BITL(4),  // Left Stick button
    HidNpadButton_StickR        = BITL(5),  // Right Stick button
    HidNpadButton_L             = BITL(6),  // L button
    HidNpadButton_R             = BITL(7),  // R button
    HidNpadButton_ZL            = BITL(8),  // ZL button
    HidNpadButton_ZR            = BITL(9),  // ZR button
    HidNpadButton_Plus          = BITL(10), // Plus button
    HidNpadButton_Minus         = BITL(11), // Minus button
    HidNpadButton_Left          = BITL(12), // D-Pad Left button
    HidNpadButton_Up            = BITL(13), // D-Pad Up button
    HidNpadButton_Right         = BITL(14), // D-Pad Right button
    HidNpadButton_Down          = BITL(15), // D-Pad Down button
    HidNpadButton_StickLLeft    = BITL(16), // Left Stick pseudo-button when moved Left
    HidNpadButton_StickLUp      = BITL(17), // Left Stick pseudo-button when moved Up
    HidNpadButton_StickLRight   = BITL(18), // Left Stick pseudo-button when moved Right
    HidNpadButton_StickLDown    = BITL(19), // Left Stick pseudo-button when moved Down
    HidNpadButton_StickRLeft    = BITL(20), // Right Stick pseudo-button when moved Left
    HidNpadButton_StickRUp      = BITL(21), // Right Stick pseudo-button when moved Up
    HidNpadButton_StickRRight   = BITL(22), // Right Stick pseudo-button when moved Right
    HidNpadButton_StickRDown    = BITL(23), // Right Stick pseudo-button when moved Left
    HidNpadButton_LeftSL        = BITL(24), // SL button on Left Joy-Con
    HidNpadButton_LeftSR        = BITL(25), // SR button on Left Joy-Con
    HidNpadButton_RightSL       = BITL(26), // SL button on Right Joy-Con
    HidNpadButton_RightSR       = BITL(27), // SR button on Right Joy-Con
    HidNpadButton_Palma         = BITL(28), // Top button on Poké Ball Plus (Palma) controller
    HidNpadButton_Verification  = BITL(29), // Verification
    HidNpadButton_HandheldLeftB = BITL(30), // B button on Left NES/HVC controller in Handheld mode
    HidNpadButton_LagonCLeft    = BITL(31), // Left C button in N64 controller
    HidNpadButton_LagonCUp      = BITL(32), // Up C button in N64 controller
    HidNpadButton_LagonCRight   = BITL(33), // Right C button in N64 controller
    HidNpadButton_LagonCDown    = BITL(34), // Down C button in N64 controller

    HidNpadButton_AnyLeft  = HidNpadButton_Left   | HidNpadButton_StickLLeft  | HidNpadButton_StickRLeft,  ///< Bitmask containing all buttons that are considered Left (D-Pad, Sticks)
    HidNpadButton_AnyUp    = HidNpadButton_Up     | HidNpadButton_StickLUp    | HidNpadButton_StickRUp,    ///< Bitmask containing all buttons that are considered Up (D-Pad, Sticks)
    HidNpadButton_AnyRight = HidNpadButton_Right  | HidNpadButton_StickLRight | HidNpadButton_StickRRight, ///< Bitmask containing all buttons that are considered Right (D-Pad, Sticks)
    HidNpadButton_AnyDown  = HidNpadButton_Down   | HidNpadButton_StickLDown  | HidNpadButton_StickRDown,  ///< Bitmask containing all buttons that are considered Down (D-Pad, Sticks)
    HidNpadButton_AnySL    = HidNpadButton_LeftSL | HidNpadButton_RightSL,                                 ///< Bitmask containing SL buttons on both Joy-Cons (Left/Right)
    HidNpadButton_AnySR    = HidNpadButton_LeftSR | HidNpadButton_RightSR,                                 ///< Bitmask containing SR buttons on both Joy-Cons (Left/Right)
} HidNpadButton;
#endif


#endif
