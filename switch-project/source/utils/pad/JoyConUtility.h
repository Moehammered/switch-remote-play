#ifndef __JOYCONUTILITY_H__
#define __JOYCONUTILITY_H__

#include "JoyConMapping.h"
extern "C"
{
    #include <switch/runtime/pad.h>
}

namespace joyconutility
{
    void RemapKey(PadState & pad, HidNpadButton const from, HidNpadButton const to);
    void ClearAnalogSticks(PadState & pad);
    void ClearShoulderButtons(PadState & pad);

    HidAnalogStickState const RotateAnalogAxis(HidAnalogStickState const analogStick, JoyConAnalogRotation const rotation);
    JoyConMapping const CreateJoyConMapping(uint32_t const analogFlags, uint32_t const buttonFlags);
    PadState const TranslateJoyConPad(PadState const & pad, JoyConMapping const options);
}

#endif
