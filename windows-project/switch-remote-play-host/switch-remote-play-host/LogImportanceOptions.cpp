#include "LogImportanceOptions.h"
#include "srp/utils/EnumMapper.h"

std::string LogImportanceToStr(LogImportance const val)
{
    return enumToStr(LogImportanceDesc, val);
}

LogImportance LogImportanceFromStr(std::string const str)
{
    return strToEnum(LogImportanceDesc, str);
}