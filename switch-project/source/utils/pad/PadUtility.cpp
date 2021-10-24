#include "PadUtility.h"
#include <unordered_map>
#include <sstream>
#include <vector>

extern "C"
{
    #include <switch/services/hid.h>
}

namespace
{
    template<typename numberType>
    std::string bitString(numberType val, int bitLength = 32)
    {
        auto bits = std::string{};

        for(auto i = 0; i < bitLength; ++i)
        {
            auto flag = 1 << i;
            auto bit = val & flag;
            auto bitStr = bit == 0 ? "0" : "1";
            bits = bitStr + bits;
        }

        return bits;
    }

    auto constexpr Bit(uint8_t position)
    {
        return 1U << position;
    }
}

namespace padutility
{
    std::unordered_map<uint32_t, int32_t> const PadIDPortNoMap
    {
        { Bit(0), 1 },
        { Bit(1), 2 },
        { Bit(2), 3 },
        { Bit(3), 4 },
        { Bit(4), 5 },
        { Bit(5), 6 },
        { Bit(6), 7 },
        { Bit(7), 8 },
        { HidNpadIdType_Handheld, 1 }
    };

    std::unordered_map<uint32_t, std::string> const PadIDPortStrMap
    {
        { Bit(0), "Pad No.1" },
        { Bit(1), "Pad No.2" },
        { Bit(2), "Pad No.3" },
        { Bit(3), "Pad No.4" },
        { Bit(4), "Pad No.5" },
        { Bit(5), "Pad No.6" },
        { Bit(6), "Pad No.7" },
        { Bit(7), "Pad No.8" },
        { HidNpadIdType_Handheld, "Handheld" }
    };

    std::unordered_map<HidDeviceTypeBits, std::string> const DeviceTypeMap
    {
        { HidDeviceTypeBits::HidDeviceTypeBits_FullKey, "Pro/GC Controller" },
        { HidDeviceTypeBits::HidDeviceTypeBits_Unknown1, "Unknown Controller" },
        { HidDeviceTypeBits::HidDeviceTypeBits_HandheldLeft, "Handheld Left" },
        { HidDeviceTypeBits::HidDeviceTypeBits_HandheldRight, "Handheld Right" },
        { HidDeviceTypeBits::HidDeviceTypeBits_JoyLeft, "JoyCon Left" },
        { HidDeviceTypeBits::HidDeviceTypeBits_JoyRight, "JoyCon Right" },
        { HidDeviceTypeBits::HidDeviceTypeBits_Palma, "Pokeball Plus Controller" },
        { HidDeviceTypeBits::HidDeviceTypeBits_LarkLeftHVC, "Famicom Left" },
        { HidDeviceTypeBits::HidDeviceTypeBits_LarkRightHVC, "Famicom Right" },
        { HidDeviceTypeBits::HidDeviceTypeBits_LarkLeftNES, "NES Left" },
        { HidDeviceTypeBits::HidDeviceTypeBits_LarkRightNES, "NES Right" },
        { HidDeviceTypeBits::HidDeviceTypeBits_SystemExt, "Generic External Controller" },
        { HidDeviceTypeBits::HidDeviceTypeBits_System, "Generic Controller" }
    };

    std::vector<PadState> initialisePads(uint32_t max)
    {
        auto controllerCount = max < maxControllers ? max : maxControllers;
        padConfigureInput(controllerCount, HidNpadStyleSet_NpadStandard);

        auto controllerPads = std::vector<PadState>(maxControllers);
        
        for(auto i = 0U; i < controllerCount && i <= HidNpadIdType_No8; ++i)
        {
            auto& pad = controllerPads[i];
            if(i == HidNpadIdType_No1)
                padInitializeDefault(&pad);
            else
            {   
                auto padMaskID = 1 << i;
                padInitializeWithMask(&pad, padMaskID);
            }
        }

        return controllerPads;
    }

    std::string padIDToPort(int id)
    {
        auto match = PadIDPortStrMap.find((HidNpadIdType)id);

        if(match != PadIDPortStrMap.end())
            return match->second;
        else
            return "ID Unknown: " + bitString(id);
    }

    std::string padDeviceTypeStr(HidDeviceTypeBits deviceType)
    {
        auto match = DeviceTypeMap.find(deviceType);

        if(match != DeviceTypeMap.end())
            return match->second;
        else
            return "Device Type Unknown: " + bitString(deviceType);
    }

    uint32_t padIDToHidNpadIdType(int id)
    {
        auto copy = id >> 1;
        if(copy <= HidNpadIdType::HidNpadIdType_No8)
            return copy;
        else
            return id;
    }

    bool deviceTypeIdIsJoyCon(HidDeviceTypeBits deviceType)
    {
        return deviceType == HidDeviceTypeBits::HidDeviceTypeBits_JoyLeft
            || deviceType == HidDeviceTypeBits::HidDeviceTypeBits_JoyRight;
    }

    bool joyconIsHorizontal(int id)
    {
        auto padSystemProps = HidNpadSystemProperties{};
        auto padTypeID = (HidNpadIdType)padIDToHidNpadIdType(id);

        hidGetNpadSystemProperties(padTypeID, &padSystemProps);
        return padSystemProps.is_abxy_button_oriented;
    }

    std::string padPropertiesToString(int id)
    {
        auto padSystemProps = HidNpadSystemProperties{};
        auto padTypeID = (HidNpadIdType)padIDToHidNpadIdType(id);

        hidGetNpadSystemProperties(padTypeID, &padSystemProps);

        auto ss = std::stringstream{};

        if(padSystemProps.is_charging)
            ss << "charging | ";
        if(!padSystemProps.is_directional_buttons_available)
            ss << "dpad N/A | ";
        if(!padSystemProps.is_minus_available)
            ss << "'-' N/A | ";
        if(!padSystemProps.is_plus_available)
            ss << "'+' N/A | ";
        if(padSystemProps.is_abxy_button_oriented)
            ss << "ABXY Ori. | ";
        if(padSystemProps.is_sl_sr_button_oriented)
            ss << "SLSR Ori.";

        return ss.str();
    }

    int32_t padNumber(PadState const & pad)
    {
        auto match = PadIDPortNoMap.find(pad.id_mask);
        if(match != PadIDPortNoMap.end())
            return match->second;
        else
            return -1;
    }

    HidDeviceTypeBits const padDeviceType(PadState const & pad)
    {
        auto padHidNPad = (HidNpadIdType)padIDToHidNpadIdType(pad.id_mask);
        return (HidDeviceTypeBits)hidGetNpadDeviceType(padHidNPad);
    }

    bool padIsSingleJoyCon(PadState const & pad)
    {
        auto deviceType = padDeviceType(pad);
        return deviceTypeIdIsJoyCon(deviceType);
    }

    bool isLeftJoyCon(PadState const & pad)
    {
        auto deviceTypeID = padDeviceType(pad);
        return deviceTypeID == HidDeviceTypeBits_JoyLeft;
    }

    bool isRightJoyCon(PadState const & pad)
    {
        auto deviceTypeID = padDeviceType(pad);
        return deviceTypeID == HidDeviceTypeBits_JoyRight;
    }
}
