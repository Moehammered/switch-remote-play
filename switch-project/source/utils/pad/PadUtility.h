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
    
    std::vector<PadState> InitialisePads(uint32_t max);

    std::string PadIDToPort(int id);
    std::string PadDeviceTypeStr(HidDeviceTypeBits deviceID);
    uint32_t PadIDToHidNpadIdType(int id);

    bool DeviceTypeIdIsJoyCon(HidDeviceTypeBits deviceType);
    bool JoyconIsHorizontal(int id);
    std::string PadPropertiesToString(int id);

    int32_t PadNumber(PadState const & pad);
    HidDeviceTypeBits const PadDeviceType(PadState const & pad);
    bool PadIsSingleJoyCon(PadState const & pad);
    bool IsLeftJoyCon(PadState const & pad);
    bool IsRightJoyCon(PadState const & pad);
}

#endif