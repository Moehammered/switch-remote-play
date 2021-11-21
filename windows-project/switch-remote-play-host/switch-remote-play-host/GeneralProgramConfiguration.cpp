#include "GeneralProgramConfiguration.h"

GeneralProgramConfiguration::GeneralProgramConfiguration()
    : configMap{ "config.ini", general::ProgramParametersDesc }
{
}

GeneralProgramConfiguration::GeneralProgramConfiguration(std::string const file)
    : configMap{ file, general::ProgramParametersDesc }
{
}

bool GeneralProgramConfiguration::Save(general::GeneralProgramConfig const data)
{
    return configMap.Save(data, general::ProgramParamsToStr);
}

general::GeneralProgramConfig const GeneralProgramConfiguration::Data() const
{
    return configMap.Data(general::ProgramParamsFromStr);
}
