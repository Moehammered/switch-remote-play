#ifndef __PADUTILITY_H__
#define __PADUTILITY_H__

#include <vector>
#include <string>
extern "C"
{
    #include <switch/runtime/pad.h>
}

namespace padutility
{
    uint32_t constexpr maxControllers = 8;
    auto constexpr leftAnalogIndex = 0;
    auto constexpr rightAnalogIndex = 1;
    
    std::vector<PadState> initialisePads(uint32_t max);

    std::string padIDToPort(int id);
    std::string padDeviceTypeStr(HidDeviceTypeBits deviceID);
    uint32_t padIDToHidNpadIdType(int id);

    bool deviceTypeIdIsJoyCon(HidDeviceTypeBits deviceType);
    bool joyconIsHorizontal(int id);
    std::string padPropertiesToString(int id);

    int32_t padNumber(PadState const & pad);
    HidDeviceTypeBits const padDeviceType(PadState const & pad);
    bool padIsSingleJoyCon(PadState const & pad);
    bool isLeftJoyCon(PadState const & pad);
    bool isRightJoyCon(PadState const & pad);
}

#endif