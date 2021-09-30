#include "JoyConUtility.h"
#include <unordered_map>
#include "PadUtility.h"

namespace 
{
    auto constexpr leftAnalogIndex = padutility::leftAnalogIndex;
    auto constexpr rightAnalogIndex = padutility::rightAnalogIndex;

    int64_t constexpr sqrMagnitude(HidAnalogStickState const & analog)
    {
        return analog.x * analog.x + analog.y * analog.y;
    }

    HidAnalogStickState const DominantStick(HidAnalogStickState const & a, HidAnalogStickState const & b)
    {
        auto lengthA = sqrMagnitude(a);
        auto lengthB = sqrMagnitude(b);

        return lengthA < lengthB ? b : a;
    }

    std::unordered_map<HidNpadButton, HidNpadButton> const AnalogBtnOrientationMap
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

    std::unordered_map<HidNpadButton, HidNpadButton> const RightAnalogToLeftBtnMap
    {
        { HidNpadButton::HidNpadButton_StickRLeft, HidNpadButton::HidNpadButton_StickLLeft },
        { HidNpadButton::HidNpadButton_StickRRight, HidNpadButton::HidNpadButton_StickLRight },
        { HidNpadButton::HidNpadButton_StickRUp, HidNpadButton::HidNpadButton_StickLUp },
        { HidNpadButton::HidNpadButton_StickRDown, HidNpadButton::HidNpadButton_StickLDown },
        { HidNpadButton::HidNpadButton_StickR, HidNpadButton::HidNpadButton_StickL }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const LeftAnalogToRightBtnMap
    {
        { HidNpadButton::HidNpadButton_StickLLeft, HidNpadButton::HidNpadButton_StickRLeft },
        { HidNpadButton::HidNpadButton_StickLRight, HidNpadButton::HidNpadButton_StickRRight },
        { HidNpadButton::HidNpadButton_StickLUp, HidNpadButton::HidNpadButton_StickRUp },
        { HidNpadButton::HidNpadButton_StickLDown, HidNpadButton::HidNpadButton_StickRDown },
        { HidNpadButton::HidNpadButton_StickL, HidNpadButton::HidNpadButton_StickR }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const AnalogToDpadMap
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

    std::unordered_map<HidNpadButton, HidNpadButton> const DpadToFaceButtonMap
    {
        { HidNpadButton::HidNpadButton_Right, HidNpadButton::HidNpadButton_A },
        { HidNpadButton::HidNpadButton_Down, HidNpadButton::HidNpadButton_B },
        { HidNpadButton::HidNpadButton_Up, HidNpadButton::HidNpadButton_X },
        { HidNpadButton::HidNpadButton_Left, HidNpadButton::HidNpadButton_Y }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const FaceButtonToDpadMap
    {
        { HidNpadButton::HidNpadButton_A, HidNpadButton::HidNpadButton_Right },
        { HidNpadButton::HidNpadButton_B, HidNpadButton::HidNpadButton_Down },
        { HidNpadButton::HidNpadButton_X, HidNpadButton::HidNpadButton_Up },
        { HidNpadButton::HidNpadButton_Y, HidNpadButton::HidNpadButton_Left }
    };

    std::unordered_map<HidNpadButton, HidNpadButton> const FaceButtonRotateRight90Map
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

    std::unordered_map<HidNpadButton, HidNpadButton> const FaceButtonRotateLeft90Map
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

    auto constexpr LeftAnalogDirectionFlags = HidNpadButton_StickLLeft | HidNpadButton_StickLRight
                                            | HidNpadButton_StickLUp | HidNpadButton_StickLDown;
    auto constexpr LeftAnalogBtnFlags = LeftAnalogDirectionFlags | HidNpadButton_StickL;

    auto constexpr RightAnalogDirectionFlags = HidNpadButton_StickRLeft | HidNpadButton_StickRRight
                                             | HidNpadButton_StickRUp | HidNpadButton_StickRDown;
    auto constexpr RightAnalogBtnFlags = RightAnalogDirectionFlags | HidNpadButton_StickR;

    auto constexpr ClearLeftAnalogDirectionMask = ~LeftAnalogDirectionFlags;
    auto constexpr ClearLeftAnalogBtnMask = ~LeftAnalogBtnFlags;

    auto constexpr ClearRightAnalogDirectionMask = ~RightAnalogDirectionFlags;
    auto constexpr ClearRightAnalogBtnMask = ~RightAnalogBtnFlags;

    auto constexpr ShoulderButtonFlags = HidNpadButton_L | HidNpadButton_ZL 
                                       | HidNpadButton_R | HidNpadButton_ZR;
    auto constexpr ClearShoulderButtonsMask = ~ShoulderButtonFlags;
}

namespace joyconutility
{
    HidAnalogStickState const RotateAnalogAxis(HidAnalogStickState const analogStick, JoyConAnalogRotation const rotation)
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

    void RemapKeys(PadState & pad, std::unordered_map<HidNpadButton, HidNpadButton> const & map)
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

    void RemapKey(PadState & pad, HidNpadButton const from, HidNpadButton const to)
    {
        auto curKeysMask = pad.buttons_cur & from;
        if(curKeysMask == from)
            pad.buttons_cur = (pad.buttons_cur & ~curKeysMask) | to;

        auto oldKeysMask = pad.buttons_old & from;
        if(oldKeysMask == from)
            pad.buttons_old = (pad.buttons_old & ~oldKeysMask) | to;
    }

    void ClearAnalogSticks(PadState & pad)
    {
        pad.sticks[leftAnalogIndex] = {};
        pad.sticks[rightAnalogIndex] = {};

        auto clearedCur = pad.buttons_cur & ClearLeftAnalogDirectionMask;
        clearedCur &= ClearRightAnalogDirectionMask;

        auto clearedOld = pad.buttons_cur & ClearLeftAnalogDirectionMask;
        clearedOld &= ClearRightAnalogDirectionMask;

        pad.buttons_cur = clearedCur;
        pad.buttons_old = clearedOld;
    }

    void ClearShoulderButtons(PadState & pad)
    {
        auto curKeys = pad.buttons_cur;
        auto oldKeys = pad.buttons_old;

        pad.buttons_cur = curKeys & ClearShoulderButtonsMask;
        pad.buttons_old = oldKeys & ClearShoulderButtonsMask;
    }

    JoyConMapping const CreateJoyConMapping(uint32_t const analogFlags, uint32_t const buttonFlags)
    {
        auto mapping = JoyConMapping{};

        mapping.analogFlags = analogFlags & JoyConAnalogMapping::AllAnalogOptions;
        mapping.buttonFlags = buttonFlags & JoyConButtonMapping::AllButtonOptions;

        return mapping;
    }

    void TranslateAnalogOptions(PadState & pad, uint32_t const analogFlags)
    {
        if(analogFlags & JoyConAnalogMapping::OrientAxis)
        {
            auto leftAnalog = RotateAnalogAxis(pad.sticks[leftAnalogIndex], JoyConAnalogRotation::Left90);
            auto rightAnalog = RotateAnalogAxis(pad.sticks[rightAnalogIndex], JoyConAnalogRotation::Right90);
            
            pad.sticks[leftAnalogIndex] = leftAnalog;
            pad.sticks[rightAnalogIndex] = rightAnalog;

            RemapKeys(pad, AnalogBtnOrientationMap);
        }
        if((analogFlags & JoyConAnalogMapping::AssignToBoth) == JoyConAnalogMapping::AssignToBoth)
        {
            auto priorityStick = DominantStick(pad.sticks[leftAnalogIndex], pad.sticks[rightAnalogIndex]);
            pad.sticks[leftAnalogIndex] = priorityStick;
            pad.sticks[rightAnalogIndex] = priorityStick;
        }
        else if(analogFlags & JoyConAnalogMapping::AssignToLeft)
        {
            auto priorityStick = DominantStick(pad.sticks[leftAnalogIndex], pad.sticks[rightAnalogIndex]);
            pad.sticks[leftAnalogIndex] = priorityStick;
            pad.sticks[rightAnalogIndex] = {};

            RemapKeys(pad, RightAnalogToLeftBtnMap);
        }
        else if(analogFlags & JoyConAnalogMapping::AssignToRight)
        {
            auto priorityStick = DominantStick(pad.sticks[leftAnalogIndex], pad.sticks[rightAnalogIndex]);
            pad.sticks[leftAnalogIndex] = {};
            pad.sticks[rightAnalogIndex] = priorityStick;
            
            RemapKeys(pad, LeftAnalogToRightBtnMap);
        }
        if(analogFlags & JoyConAnalogMapping::AssignToDpad)
        {
            RemapKeys(pad, AnalogToDpadMap);
        }
        if(analogFlags & JoyConAnalogMapping::ClearAnalogAxis)
        {
            ClearAnalogSticks(pad);
        }
    }

    void TranslateButtonOptions(PadState & pad, uint32_t const buttonFlags)
    {
        if(buttonFlags & JoyConButtonMapping::DpadToFaceButtons)
        {
            RemapKeys(pad, DpadToFaceButtonMap);
        }
        if(buttonFlags & JoyConButtonMapping::FaceButtonsToDpad)
        {
            RemapKeys(pad, FaceButtonToDpadMap);
        }
        if(buttonFlags & JoyConButtonMapping::RotateFaceButtonRight90)
        {
            RemapKeys(pad, FaceButtonRotateRight90Map);
        }
        else if(buttonFlags & JoyConButtonMapping::RotateFaceButtonLeft90)
        {
            RemapKeys(pad, FaceButtonRotateLeft90Map);
        }
        if(buttonFlags & JoyConButtonMapping::IgnoreOriginalShoulderButtons)
        {
            ClearShoulderButtons(pad);
        }
        if(buttonFlags & JoyConButtonMapping::SlSrToLR)
        {
            RemapKeys(pad, SLSRToLRMap);
        }
        if(buttonFlags & JoyConButtonMapping::SlSrToZLZR)
        {
            RemapKeys(pad, SLSRToZLZRMap);
        }
        if(buttonFlags & JoyConButtonMapping::MinusToPlus)
        {
            RemapKey(pad, HidNpadButton::HidNpadButton_Minus, HidNpadButton::HidNpadButton_Plus);
        }
        if(buttonFlags & JoyConButtonMapping::PlusToMinus)
        {   
            RemapKey(pad, HidNpadButton::HidNpadButton_Plus, HidNpadButton::HidNpadButton_Minus);
        }
    }

    PadState const TranslateJoyConPad(PadState const & pad, JoyConMapping const options)
    {
        auto copy = PadState{pad};

        auto buttonFlags = JoyConButtonMapping::AllButtonOptions & options.buttonFlags;
        if(buttonFlags != JoyConButtonMapping::NoButtonMapping)
            TranslateButtonOptions(copy, buttonFlags);

        auto analogFlags = JoyConAnalogMapping::AllAnalogOptions & options.analogFlags;
        if(analogFlags != JoyConAnalogMapping::NoAnalogMapping)
            TranslateAnalogOptions(copy, analogFlags);

        return copy;
    }
}
