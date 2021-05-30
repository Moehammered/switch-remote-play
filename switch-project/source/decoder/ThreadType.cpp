#include "ThreadType.h"
#include "../utils/EnumMapper.h"

namespace decoder
{
    std::string const ThreadTypeToString(int32_t flag)
    {
        return enumToStr(threadOptions, flag);
    }

    std::string const ThreadTypeToDescription(int32_t flag)
    {
        return enumToStr(threadDesc, flag);
    }
    
    int32_t ParseThreadType(std::string const type)
    {
        return strToEnum(threadOptions, type);
    }

    std::string const ThreadCountToString(int32_t count)
    {
        auto filtered = std::min(count, MaxThreadCount);
        filtered = std::max(MinThreadCount, count);

        return std::to_string(filtered);
    }

    int32_t ParseThreadCount(std::string const count)
    {
        if(count.empty())
            return DefaultThreadCount;
        else
        {
            auto num = std::stoi(count);
            auto filtered = std::min(num, MaxThreadCount);
            filtered = std::max(MinThreadCount, num);
            return filtered;
        }
    }
}
