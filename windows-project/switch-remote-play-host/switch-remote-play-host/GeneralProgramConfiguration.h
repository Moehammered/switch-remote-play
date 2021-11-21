#ifndef __GENERALPROGRAMCONFIGURATION_H__
#define __GENERALPROGRAMCONFIGURATION_H__

#include "GeneralProgramOptions.h"
#include "srp/utils/MappedDataConfiguration.h"
#include <string>

class GeneralProgramConfiguration
{
public:
    GeneralProgramConfiguration();
    GeneralProgramConfiguration(std::string const file);

    bool Save(general::GeneralProgramConfig const data);
    general::GeneralProgramConfig const Data() const;

private:
    MappedDataConfiguration<general::GeneralProgramConfig, general::ProgramParameters> configMap;
};

#endif
