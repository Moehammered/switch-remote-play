#ifndef __JOYCONUTILITY_H__
#define __JOYCONUTILITY_H__

#include "JoyConMapping.h"
extern "C"
{
    #include <switch/runtime/pad.h>
}

namespace joyconutility
{
    void remapKey(PadState & pad, HidNpadButton const from, HidNpadButton const to);
    void clearAnalogSticks(PadState & pad);
    void clearShoulderButtons(PadState & pad);

    HidAnalogStickState const rotateAnalogAxis(HidAnalogStickState const analogStick, JoyConAnalogRotation const rotation);
    JoyConMapping const createJoyConMapping(uint32_t const analogFlags, uint32_t const buttonFlags);
    PadState const translateJoyConPad(PadState const & pad, JoyConMapping const options);
}

#endif
