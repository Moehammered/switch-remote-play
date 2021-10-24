#include "ThreadType.h"
#include "../utils/EnumMapper.h"

namespace decoder
{
    std::string const threadTypeToString(int32_t flag)
    {
        return enumToStr(threadOptions, flag);
    }

    std::string const threadTypeToDescription(int32_t flag)
    {
        return enumToStr(threadDesc, flag);
    }
    
    int32_t parseThreadType(std::string const type)
    {
        return strToEnum(threadOptions, type);
    }

    std::string const threadCountToString(int32_t count)
    {
        auto filtered = std::min(count, maxThreadCount);
        filtered = std::max(minThreadCount, count);

        return std::to_string(filtered);
    }

    int32_t parseThreadCount(std::string const count)
    {
        if(count.empty())
            return defaultThreadCount;
        else
        {
            auto num = std::stoi(count);
            auto filtered = std::min(num, maxThreadCount);
            filtered = std::max(minThreadCount, num);
            return filtered;
        }
    }
}
