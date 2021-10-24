#include "JoyConUtility.h"
#include "PadUtility.h"
#include <unordered_map>

namespace 
{
    auto constexpr leftAnalogIndex = padutility::leftAnalogIndex;
    auto constexpr rightAnalogIndex = padutility::rightAnalogIndex;

    int64_t constexpr sqrMagnitude(HidAnalogStickState const & analog)
    {
        return analog.x * analog.x + analog.y * analog.y;
    }

    HidAnalogStickState const dominantStick(HidAnalogStickState const & a, HidAnalogStickState const & b)
    {
        auto lengthA = sqrMagnitude(a);
        auto lengthB = sqrMagnitude(b);

        return lengthA < lengthB ? b : a;
    }

    std::unordered_map<HidNpadButton, HidNpadButton> const analogBtnOrientationMap
    {
        { HidNpadButton::HidNpadButton_StickRLeft, HidNpadButton::HidNpadButton_StickRUp },
        { HidNpadButton::HidNpadButton_StickRRight, HidNpadButton::HidNpadButton_StickRDown },
        { HidNpadButton::HidNpadButton_StickRUp, HidNpadButton::HidNpadButton_StickRRight },
        { HidNpadButton::HidNpadButton_StickRDown, HidNpadButton::HidNpadButton_StickRLeft },
        { HidNpadButton::HidNpadButton_StickLLeft, HidNpadButton::HidNpadButton_StickLDown },
        { HidNpadButton::HidNpadButton_StickLRight, HidNpadButton::HidNpadButton_StickLUp },
        { HidNpadButton::HidNpadButton_StickLUp, HidNpadButton::HidNpadButton_StickLLeft },
        { HidNpadButton::HidNpadButton_StickLDown, HidNpadButton::HidNpadButton_StickLRight }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const rightAnalogToLeftBtnMap
    {
        { HidNpadButton::HidNpadButton_StickRLeft, HidNpadButton::HidNpadButton_StickLLeft },
        { HidNpadButton::HidNpadButton_StickRRight, HidNpadButton::HidNpadButton_StickLRight },
        { HidNpadButton::HidNpadButton_StickRUp, HidNpadButton::HidNpadButton_StickLUp },
        { HidNpadButton::HidNpadButton_StickRDown, HidNpadButton::HidNpadButton_StickLDown },
        { HidNpadButton::HidNpadButton_StickR, HidNpadButton::HidNpadButton_StickL }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const leftAnalogToRightBtnMap
    {
        { HidNpadButton::HidNpadButton_StickLLeft, HidNpadButton::HidNpadButton_StickRLeft },
        { HidNpadButton::HidNpadButton_StickLRight, HidNpadButton::HidNpadButton_StickRRight },
        { HidNpadButton::HidNpadButton_StickLUp, HidNpadButton::HidNpadButton_StickRUp },
        { HidNpadButton::HidNpadButton_StickLDown, HidNpadButton::HidNpadButton_StickRDown },
        { HidNpadButton::HidNpadButton_StickL, HidNpadButton::HidNpadButton_StickR }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const analogToDpadMap
    {
        { HidNpadButton::HidNpadButton_StickRLeft, HidNpadButton::HidNpadButton_Left },
        { HidNpadButton::HidNpadButton_StickRRight, HidNpadButton::HidNpadButton_Right },
        { HidNpadButton::HidNpadButton_StickRUp, HidNpadButton::HidNpadButton_Up },
        { HidNpadButton::HidNpadButton_StickRDown, HidNpadButton::HidNpadButton_Down },
        { HidNpadButton::HidNpadButton_StickLLeft, HidNpadButton::HidNpadButton_Left },
        { HidNpadButton::HidNpadButton_StickLRight, HidNpadButton::HidNpadButton_Right },
        { HidNpadButton::HidNpadButton_StickLUp, HidNpadButton::HidNpadButton_Up },
        { HidNpadButton::HidNpadButton_StickLDown, HidNpadButton::HidNpadButton_Down }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const dpadToFaceButtonMap
    {
        { HidNpadButton::HidNpadButton_Right, HidNpadButton::HidNpadButton_A },
        { HidNpadButton::HidNpadButton_Down, HidNpadButton::HidNpadButton_B },
        { HidNpadButton::HidNpadButton_Up, HidNpadButton::HidNpadButton_X },
        { HidNpadButton::HidNpadButton_Left, HidNpadButton::HidNpadButton_Y }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const faceButtonToDpadMap
    {
        { HidNpadButton::HidNpadButton_A, HidNpadButton::HidNpadButton_Right },
        { HidNpadButton::HidNpadButton_B, HidNpadButton::HidNpadButton_Down },
        { HidNpadButton::HidNpadButton_X, HidNpadButton::HidNpadButton_Up },
        { HidNpadButton::HidNpadButton_Y, HidNpadButton::HidNpadButton_Left }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const faceButtonRotateRight90Map
    {
        { HidNpadButton::HidNpadButton_X, HidNpadButton::HidNpadButton_A },
        { HidNpadButton::HidNpadButton_A, HidNpadButton::HidNpadButton_B },
        { HidNpadButton::HidNpadButton_Y, HidNpadButton::HidNpadButton_X },
        { HidNpadButton::HidNpadButton_B, HidNpadButton::HidNpadButton_Y },
        { HidNpadButton::HidNpadButton_Up, HidNpadButton::HidNpadButton_Right },
        { HidNpadButton::HidNpadButton_Right, HidNpadButton::HidNpadButton_Down },
        { HidNpadButton::HidNpadButton_Down, HidNpadButton::HidNpadButton_Left },
        { HidNpadButton::HidNpadButton_Left, HidNpadButton::HidNpadButton_Up }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const faceButtonRotateLeft90Map
    {
        { HidNpadButton::HidNpadButton_A, HidNpadButton::HidNpadButton_X },
        { HidNpadButton::HidNpadButton_B, HidNpadButton::HidNpadButton_A },
        { HidNpadButton::HidNpadButton_X, HidNpadButton::HidNpadButton_Y },
        { HidNpadButton::HidNpadButton_Y, HidNpadButton::HidNpadButton_B },
        { HidNpadButton::HidNpadButton_Up, HidNpadButton::HidNpadButton_Left },
        { HidNpadButton::HidNpadButton_Right, HidNpadButton::HidNpadButton_Up },
        { HidNpadButton::HidNpadButton_Down, HidNpadButton::HidNpadButton_Right },
        { HidNpadButton::HidNpadButton_Left, HidNpadButton::HidNpadButton_Down }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const SLSRToLRMap
    {
        { HidNpadButton::HidNpadButton_AnySL, HidNpadButton::HidNpadButton_L },
        { HidNpadButton::HidNpadButton_AnySR, HidNpadButton::HidNpadButton_R }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const SLSRToZLZRMap
    {
        { HidNpadButton::HidNpadButton_AnySL, HidNpadButton::HidNpadButton_ZL },
        { HidNpadButton::HidNpadButton_AnySR, HidNpadButton::HidNpadButton_ZR }
    };

    auto constexpr leftAnalogDirectionFlags = HidNpadButton_StickLLeft | HidNpadButton_StickLRight
                                            | HidNpadButton_StickLUp | HidNpadButton_StickLDown;
    auto constexpr leftAnalogBtnFlags = leftAnalogDirectionFlags | HidNpadButton_StickL;

    auto constexpr rightAnalogDirectionFlags = HidNpadButton_StickRLeft | HidNpadButton_StickRRight
                                             | HidNpadButton_StickRUp | HidNpadButton_StickRDown;
    auto constexpr rightAnalogBtnFlags = rightAnalogDirectionFlags | HidNpadButton_StickR;

    auto constexpr clearLeftAnalogDirectionMask = ~leftAnalogDirectionFlags;
    auto constexpr clearLeftAnalogBtnMask = ~leftAnalogBtnFlags;

    auto constexpr clearRightAnalogDirectionMask = ~rightAnalogDirectionFlags;
    auto constexpr clearRightAnalogBtnMask = ~rightAnalogBtnFlags;

    auto constexpr shoulderButtonFlags = HidNpadButton_L | HidNpadButton_ZL 
                                       | HidNpadButton_R | HidNpadButton_ZR;
    auto constexpr clearShoulderButtonsMask = ~shoulderButtonFlags;

    void remapKeys(PadState & pad, std::unordered_map<HidNpadButton, HidNpadButton> const & map)
    {
        auto curKeysToRemove = 0;
        auto curKeysToPlace = 0;

        auto oldKeysToRemove = 0;
        auto oldKeysToPlace = 0;

        for(auto const & btn : map)
        {
            if(btn.first & pad.buttons_cur)
            {
                curKeysToRemove |= btn.first;
                curKeysToPlace |= btn.second;
            }
            if(btn.first & pad.buttons_old)
            {
                oldKeysToRemove |= btn.first;
                oldKeysToPlace |= btn.second;
            }
        }

        auto currKeys = (pad.buttons_cur & ~curKeysToRemove) | curKeysToPlace;
        auto oldKeys = (pad.buttons_old & ~oldKeysToRemove) | oldKeysToPlace;
        pad.buttons_cur = currKeys;
        pad.buttons_old = oldKeys;
    }

    void translateAnalogOptions(PadState & pad, uint32_t const analogFlags)
    {
        using namespace joyconutility;

        if(analogFlags & JoyConAnalogMapping::OrientAxis)
        {
            auto leftAnalog = rotateAnalogAxis(pad.sticks[leftAnalogIndex], JoyConAnalogRotation::Left90);
            auto rightAnalog = rotateAnalogAxis(pad.sticks[rightAnalogIndex], JoyConAnalogRotation::Right90);
            
            pad.sticks[leftAnalogIndex] = leftAnalog;
            pad.sticks[rightAnalogIndex] = rightAnalog;

            remapKeys(pad, analogBtnOrientationMap);
        }
        if((analogFlags & JoyConAnalogMapping::AssignToBoth) == JoyConAnalogMapping::AssignToBoth)
        {
            auto priorityStick = dominantStick(pad.sticks[leftAnalogIndex], pad.sticks[rightAnalogIndex]);
            pad.sticks[leftAnalogIndex] = priorityStick;
            pad.sticks[rightAnalogIndex] = priorityStick;
        }
        else if(analogFlags & JoyConAnalogMapping::AssignToLeft)
        {
            auto priorityStick = dominantStick(pad.sticks[leftAnalogIndex], pad.sticks[rightAnalogIndex]);
            pad.sticks[leftAnalogIndex] = priorityStick;
            pad.sticks[rightAnalogIndex] = {};

            remapKeys(pad, rightAnalogToLeftBtnMap);
        }
        else if(analogFlags & JoyConAnalogMapping::AssignToRight)
        {
            auto priorityStick = dominantStick(pad.sticks[leftAnalogIndex], pad.sticks[rightAnalogIndex]);
            pad.sticks[leftAnalogIndex] = {};
            pad.sticks[rightAnalogIndex] = priorityStick;
            
            remapKeys(pad, leftAnalogToRightBtnMap);
        }
        if(analogFlags & JoyConAnalogMapping::AssignToDpad)
        {
            remapKeys(pad, analogToDpadMap);
        }
        if(analogFlags & JoyConAnalogMapping::ClearAnalogAxis)
        {
            clearAnalogSticks(pad);
        }
    }

    void translateButtonOptions(PadState & pad, uint32_t const buttonFlags)
    {
        using namespace joyconutility;
        
        if(buttonFlags & JoyConButtonMapping::DpadToFaceButtons)
        {
            remapKeys(pad, dpadToFaceButtonMap);
        }
        if(buttonFlags & JoyConButtonMapping::FaceButtonsToDpad)
        {
            remapKeys(pad, faceButtonToDpadMap);
        }
        if(buttonFlags & JoyConButtonMapping::RotateFaceButtonRight90)
        {
            remapKeys(pad, faceButtonRotateRight90Map);
        }
        else if(buttonFlags & JoyConButtonMapping::RotateFaceButtonLeft90)
        {
            remapKeys(pad, faceButtonRotateLeft90Map);
        }
        if(buttonFlags & JoyConButtonMapping::IgnoreOriginalShoulderButtons)
        {
            clearShoulderButtons(pad);
        }
        if(buttonFlags & JoyConButtonMapping::SlSrToLR)
        {
            remapKeys(pad, SLSRToLRMap);
        }
        if(buttonFlags & JoyConButtonMapping::SlSrToZLZR)
        {
            remapKeys(pad, SLSRToZLZRMap);
        }
        if(buttonFlags & JoyConButtonMapping::MinusToPlus)
        {
            remapKey(pad, HidNpadButton::HidNpadButton_Minus, HidNpadButton::HidNpadButton_Plus);
        }
        if(buttonFlags & JoyConButtonMapping::PlusToMinus)
        {   
            remapKey(pad, HidNpadButton::HidNpadButton_Plus, HidNpadButton::HidNpadButton_Minus);
        }
    }
}

namespace joyconutility
{
    void remapKey(PadState & pad, HidNpadButton const from, HidNpadButton const to)
    {
        auto curKeysMask = pad.buttons_cur & from;
        if(curKeysMask == from)
            pad.buttons_cur = (pad.buttons_cur & ~curKeysMask) | to;

        auto oldKeysMask = pad.buttons_old & from;
        if(oldKeysMask == from)
            pad.buttons_old = (pad.buttons_old & ~oldKeysMask) | to;
    }

    void clearAnalogSticks(PadState & pad)
    {
        pad.sticks[leftAnalogIndex] = {};
        pad.sticks[rightAnalogIndex] = {};

        auto clearedCur = pad.buttons_cur & clearLeftAnalogDirectionMask;
        clearedCur &= clearRightAnalogDirectionMask;

        auto clearedOld = pad.buttons_cur & clearLeftAnalogDirectionMask;
        clearedOld &= clearRightAnalogDirectionMask;

        pad.buttons_cur = clearedCur;
        pad.buttons_old = clearedOld;
    }

    void clearShoulderButtons(PadState & pad)
    {
        auto curKeys = pad.buttons_cur;
        auto oldKeys = pad.buttons_old;

        pad.buttons_cur = curKeys & clearShoulderButtonsMask;
        pad.buttons_old = oldKeys & clearShoulderButtonsMask;
    }

    HidAnalogStickState const rotateAnalogAxis(HidAnalogStickState const analogStick, JoyConAnalogRotation const rotation)
    {
        switch(rotation)
        {
            case JoyConAnalogRotation::None:
            default:
                return analogStick;

            case JoyConAnalogRotation::Left90:
            {
                auto copy = HidAnalogStickState{};
                copy.x = -analogStick.y;
                copy.y = analogStick.x;
                return copy;
            }

            case JoyConAnalogRotation::Right90:
            {
                auto copy = HidAnalogStickState{};
                copy.x = analogStick.y;
                copy.y = -analogStick.x;
                return copy;
            }

            case JoyConAnalogRotation::UpsideDown:
            {
                auto copy = HidAnalogStickState{};
                copy.x = -analogStick.y;
                copy.y = -analogStick.x;
                return copy;
            }
        }
    }

    JoyConMapping const createJoyConMapping(uint32_t const analogFlags, uint32_t const buttonFlags)
    {
        auto mapping = JoyConMapping{};

        mapping.analogFlags = analogFlags & JoyConAnalogMapping::AllAnalogOptions;
        mapping.buttonFlags = buttonFlags & JoyConButtonMapping::AllButtonOptions;

        return mapping;
    }

    PadState const translateJoyConPad(PadState const & pad, JoyConMapping const options)
    {
        auto copy = PadState{pad};

        auto buttonFlags = JoyConButtonMapping::AllButtonOptions & options.buttonFlags;
        if(buttonFlags != JoyConButtonMapping::NoButtonMapping)
            translateButtonOptions(copy, buttonFlags);

        auto analogFlags = JoyConAnalogMapping::AllAnalogOptions & options.analogFlags;
        if(analogFlags != JoyConAnalogMapping::NoAnalogMapping)
            translateAnalogOptions(copy, analogFlags);

        return copy;
    }
}
