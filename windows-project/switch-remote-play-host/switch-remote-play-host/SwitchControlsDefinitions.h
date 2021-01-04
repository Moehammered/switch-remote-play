#pragma once
#include <stdint.h>

#ifndef BIT
#define BIT(n) (1U<<(n))
#endif

typedef enum {
    HidNpadButton_A = BIT(0),  ///< A button / Right face button
    HidNpadButton_B = BIT(1),  ///< B button / Down face button
    HidNpadButton_X = BIT(2),  ///< X button / Up face button
    HidNpadButton_Y = BIT(3),  ///< Y button / Left face button
    HidNpadButton_StickL = BIT(4),  ///< Left Stick button
    HidNpadButton_StickR = BIT(5),  ///< Right Stick button
    HidNpadButton_L = BIT(6),  ///< L button
    HidNpadButton_R = BIT(7),  ///< R button
    HidNpadButton_ZL = BIT(8),  ///< ZL button
    HidNpadButton_ZR = BIT(9),  ///< ZR button
    HidNpadButton_Plus = BIT(10), ///< Plus button
    HidNpadButton_Minus = BIT(11), ///< Minus button
    HidNpadButton_Left = BIT(12), ///< D-Pad Left button
    HidNpadButton_Up = BIT(13), ///< D-Pad Up button
    HidNpadButton_Right = BIT(14), ///< D-Pad Right button
    HidNpadButton_Down = BIT(15), ///< D-Pad Down button
    HidNpadButton_StickLLeft = BIT(16), ///< Left Stick pseudo-button when moved Left
    HidNpadButton_StickLUp = BIT(17), ///< Left Stick pseudo-button when moved Up
    HidNpadButton_StickLRight = BIT(18), ///< Left Stick pseudo-button when moved Right
    HidNpadButton_StickLDown = BIT(19), ///< Left Stick pseudo-button when moved Down
    HidNpadButton_StickRLeft = BIT(20), ///< Right Stick pseudo-button when moved Left
    HidNpadButton_StickRUp = BIT(21), ///< Right Stick pseudo-button when moved Up
    HidNpadButton_StickRRight = BIT(22), ///< Right Stick pseudo-button when moved Right
    HidNpadButton_StickRDown = BIT(23), ///< Right Stick pseudo-button when moved Left
    HidNpadButton_LeftSL = BIT(24), ///< SL button on Left Joy-Con
    HidNpadButton_LeftSR = BIT(25), ///< SR button on Left Joy-Con
    HidNpadButton_RightSL = BIT(26), ///< SL button on Right Joy-Con
    HidNpadButton_RightSR = BIT(27), ///< SR button on Right Joy-Con
    HidNpadButton_Palma = BIT(28), ///< Top button on Poke Ball Plus (Palma) controller
    HidNpadButton_29 = BIT(29),
    HidNpadButton_HandheldLeftB = BIT(30), ///< B button on Left NES/HVC controller in Handheld mode

    HidNpadButton_AnyLeft = HidNpadButton_Left | HidNpadButton_StickLLeft | HidNpadButton_StickRLeft,  ///< Bitmask containing all buttons that are considered Left (D-Pad, Sticks)
    HidNpadButton_AnyUp = HidNpadButton_Up | HidNpadButton_StickLUp | HidNpadButton_StickRUp,    ///< Bitmask containing all buttons that are considered Up (D-Pad, Sticks)
    HidNpadButton_AnyRight = HidNpadButton_Right | HidNpadButton_StickLRight | HidNpadButton_StickRRight, ///< Bitmask containing all buttons that are considered Right (D-Pad, Sticks)
    HidNpadButton_AnyDown = HidNpadButton_Down | HidNpadButton_StickLDown | HidNpadButton_StickRDown,  ///< Bitmask containing all buttons that are considered Down (D-Pad, Sticks)
    HidNpadButton_AnySL = HidNpadButton_LeftSL | HidNpadButton_RightSL,                                 ///< Bitmask containing SL buttons on both Joy-Cons (Left/Right)
    HidNpadButton_AnySR = HidNpadButton_LeftSR | HidNpadButton_RightSR,                                 ///< Bitmask containing SR buttons on both Joy-Cons (Left/Right)
} HidNpadButton;

/// HidControllerKeys \deprecated
typedef enum DEPRECATED {
    KEY_A = HidNpadButton_A,
    KEY_B = HidNpadButton_B,
    KEY_X = HidNpadButton_X,
    KEY_Y = HidNpadButton_Y,
    KEY_LSTICK = HidNpadButton_StickL,
    KEY_RSTICK = HidNpadButton_StickR,
    KEY_L = HidNpadButton_L,
    KEY_R = HidNpadButton_R,
    KEY_ZL = HidNpadButton_ZL,
    KEY_ZR = HidNpadButton_ZR,
    KEY_PLUS = HidNpadButton_Plus,
    KEY_MINUS = HidNpadButton_Minus,
    KEY_DLEFT = HidNpadButton_Left,
    KEY_DUP = HidNpadButton_Up,
    KEY_DRIGHT = HidNpadButton_Right,
    KEY_DDOWN = HidNpadButton_Down,
    KEY_LSTICK_LEFT = HidNpadButton_StickLLeft,
    KEY_LSTICK_UP = HidNpadButton_StickLUp,
    KEY_LSTICK_RIGHT = HidNpadButton_StickLRight,
    KEY_LSTICK_DOWN = HidNpadButton_StickLDown,
    KEY_RSTICK_LEFT = HidNpadButton_StickRLeft,
    KEY_RSTICK_UP = HidNpadButton_StickRUp,
    KEY_RSTICK_RIGHT = HidNpadButton_StickRRight,
    KEY_RSTICK_DOWN = HidNpadButton_StickRDown,
    KEY_SL_LEFT = HidNpadButton_LeftSL,
    KEY_SR_LEFT = HidNpadButton_LeftSR,
    KEY_SL_RIGHT = HidNpadButton_RightSL,
    KEY_SR_RIGHT = HidNpadButton_RightSR,
    KEY_NES_HANDHELD_LEFT_B = HidNpadButton_HandheldLeftB,

    KEY_HOME = BIT(18),      ///< HOME button, only available for use with HiddbgHdlsState::buttons.
    KEY_CAPTURE = BIT(19),      ///< Capture button, only available for use with HiddbgHdlsState::buttons.
    KEY_TOUCH = BIT(28),      ///< Pseudo-key for at least one finger on the touch screen

    KEY_JOYCON_RIGHT = HidNpadButton_A,
    KEY_JOYCON_DOWN = HidNpadButton_B,
    KEY_JOYCON_UP = HidNpadButton_X,
    KEY_JOYCON_LEFT = HidNpadButton_Y,

    KEY_UP = HidNpadButton_AnyUp,
    KEY_DOWN = HidNpadButton_AnyDown,
    KEY_LEFT = HidNpadButton_AnyLeft,
    KEY_RIGHT = HidNpadButton_AnyRight,
    KEY_SL = HidNpadButton_AnySL,
    KEY_SR = HidNpadButton_AnySR,
} HidControllerKeys;

typedef struct HidVector {
    float x;
    float y;
    float z;
} HidVector;

struct alignas(8) GamepadDataPayload
{
    uint32_t keys;
    int32_t ljx, ljy;
    int32_t rjx, rjy;
    HidVector gryo;
    HidVector accelerometer;
    char padding[64 - sizeof(uint32_t) - sizeof(int32_t) * 4 - sizeof(HidVector) * 2];
};

auto constexpr GamepadDataPayloadSize = sizeof(GamepadDataPayload);