#ifndef __JOYCONMAPPING_H__
#define __JOYCONMAPPING_H__

#include <stdint.h>

namespace joyconutility
{
    enum JoyConAnalogMapping
    {
        NoAnalogMapping = 0,
        OrientAxis = 1,
        AssignToLeft = 1 << 1,
        AssignToRight = 1 << 2,
        AssignToDpad = 1 << 3,
        ClearAnalogAxis = 1 << 4,

        AssignToBoth = AssignToLeft | AssignToRight,

        AllAnalogOptions = NoAnalogMapping | OrientAxis 
            | AssignToLeft | AssignToRight 
            | AssignToDpad | ClearAnalogAxis,

        DefaultLeftJoyConAnalogRemap = OrientAxis,

        DefaultRightJoyConAnalogRemap = OrientAxis | AssignToLeft
    };

    enum JoyConButtonMapping
    {
        NoButtonMapping = 0,
        RotateFaceButtonRight90 = 1,
        RotateFaceButtonLeft90 = 1 << 1,
        DpadToFaceButtons = 1 << 2,
        FaceButtonsToDpad = 1 << 3,
        SlSrToLR = 1 << 4,
        SlSrToZLZR = 1 << 5,
        MinusToPlus = 1 << 6,
        PlusToMinus = 1 << 7,
        IgnoreOriginalShoulderButtons = 1 << 8,

        AllButtonOptions = NoButtonMapping | RotateFaceButtonRight90 | RotateFaceButtonLeft90 
            | DpadToFaceButtons | FaceButtonsToDpad
            | SlSrToLR | SlSrToZLZR 
            | MinusToPlus | PlusToMinus | IgnoreOriginalShoulderButtons,

        DefaultLeftJoyConButtonRemap = MinusToPlus | SlSrToLR 
            | DpadToFaceButtons | RotateFaceButtonLeft90
            | IgnoreOriginalShoulderButtons,

        DefaultRightJoyConButtonRemap = IgnoreOriginalShoulderButtons
            | SlSrToLR | RotateFaceButtonRight90
    };

    struct JoyConMapping
    {
        uint32_t analogFlags;
        uint32_t buttonFlags;
    };

    auto constexpr defaultLeftJoyConMapping = JoyConMapping
    {
        .analogFlags = DefaultLeftJoyConAnalogRemap,
        .buttonFlags = DefaultLeftJoyConButtonRemap
    };

    auto constexpr defaultRightJoyConMapping = JoyConMapping
    {
        .analogFlags = DefaultRightJoyConAnalogRemap,
        .buttonFlags = DefaultRightJoyConButtonRemap
    };

    enum class JoyConAnalogRotation
    {
        None,
        Left90,
        Right90,
        UpsideDown
    };
}

#endif
