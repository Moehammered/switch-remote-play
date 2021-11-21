#ifndef __GENERALPROGRAMOPTIONS_H__
#define __GENERALPROGRAMOPTIONS_H__

#include <unordered_map>
#include <string>

namespace general
{
    enum class ProgramParameters
    {
        MinimiseOnConnect,
        MuteOnConnect,
        RestoreOnDisconnect,
        RestoreMuteOnDisconnect,
        LogToFile
    };

    static std::unordered_map<ProgramParameters, std::string> const ProgramParametersDesc
    {
        { ProgramParameters::MinimiseOnConnect, "Minimise on Connect" },
        { ProgramParameters::MuteOnConnect, "Mute PC on Connect" },
        { ProgramParameters::RestoreOnDisconnect, "Restore Window on Disconnect" },
        { ProgramParameters::RestoreMuteOnDisconnect, "Restore PC Mute State on Disconnect" },
        { ProgramParameters::LogToFile, "Log to File" }
    };

    auto constexpr DefaultMinimiseOnConnect = true;
    auto constexpr DefaultMuteOnConnect = true;
    auto constexpr DefaultRestoreOnDisconnect = false;
    auto constexpr DefaultRestoreMuteOnDisconnect = false;
    auto constexpr DefaultLogToFile = false;

    struct GeneralProgramConfig
    {
        bool minimiseOnConnect;
        bool muteOnConnect;
        bool restoreOnDisconnect;
        bool restoreMuteOnDisconnect;
        bool logToFile;
    };

    auto constexpr GeneralProgramConfigSize = sizeof(GeneralProgramConfig);

    std::unordered_map<ProgramParameters, std::string> const ProgramParamsToStr(GeneralProgramConfig const config);
    GeneralProgramConfig const ProgramParamsFromStr(std::unordered_map<ProgramParameters, std::string> const& map);
}

#endif
