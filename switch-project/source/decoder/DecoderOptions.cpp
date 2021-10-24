#include "DecoderOptions.h"
#include "DecoderFlag.h"
#include "DecoderAccel.h"
#include "DiscardType.h"
#include "ThreadType.h"
#include <sstream>
#include "../utils/StringUtils.h"

std::unordered_map<DecoderParameters, std::string> decoderParamsToStr(DecoderData const data)
{
    auto values = std::unordered_map<DecoderParameters, std::string>{};

    auto f1Values = decoder::flags1ToStrings(data.flags1);
    auto f2Values = decoder::flags2ToStrings(data.flags2);

    auto csv = [](auto const & map)
    {
        std::stringstream ss{};

        for(auto const & pair : map)
            ss << pair.second << ",";

        auto output = ss.str();
        return output.substr(0, output.length()-1);
    };
    
    values[DecoderParameters::Flags1] = csv(f1Values);
    values[DecoderParameters::Flags2] = csv(f2Values);

    auto accValues = decoder::accelFlagsToStrings(data.accelFlags);
    values[DecoderParameters::AccelFlags] = csv(accValues);

    auto discVal = decoder::avDiscardToString(data.discardFilter);
    values[DecoderParameters::DiscardFilter] = discVal;

    auto typeVal = decoder::threadTypeToString(data.threadType);
    values[DecoderParameters::ThreadType] = typeVal;

    auto threadCount = decoder::threadCountToString(data.threadCount);
    values[DecoderParameters::ThreadCount] = threadCount;

    return values;
}

DecoderData decoderParamsFromStr(std::unordered_map<DecoderParameters, std::string> const & map)
{
    auto data = DecoderData();

    auto parse = [&](DecoderParameters p, auto& member, auto def, auto convert)
    {
        auto itr = map.find(p);
        if (itr != map.end())
            member = convert(itr->second);
        else
            member = def;
    };

    auto csvToList = [](auto const & csv)
    {
        std::stringstream ss(csv);
        std::string val{};
        auto list = std::vector<std::string>{};
        while(std::getline(ss, val, ','))
            list.emplace_back(val);
        
        return list;
    };

    auto parseFlags1 = [&](auto const & csv)
    {
        auto flags = csvToList(csv);
        return decoder::parseFlags1Strings(flags);
    };

    auto parseFlags2 = [&](auto const & csv)
    {
        auto flags = csvToList(csv);
        return decoder::parseFlags2Strings(flags);
    };

    auto parseAccelFlags = [&](auto const & csv)
    {
        auto flags = csvToList(csv);
        return decoder::parseAccelFlagStrings(flags);
    };

    auto parseDiscard = [](auto const & val)
    {
        return decoder::parseAVDiscardString(val);
    };

    auto parseThreadType = [](auto const & val)
    {
        return decoder::parseThreadType(val);
    };

    parse(DecoderParameters::Flags1, data.flags1, 0, parseFlags1);
    parse(DecoderParameters::Flags2, data.flags2, 0, parseFlags2);
    parse(DecoderParameters::AccelFlags, data.accelFlags, 0, parseAccelFlags);
    parse(DecoderParameters::DiscardFilter, data.discardFilter, AVDiscard::AVDISCARD_DEFAULT, parseDiscard);
    parse(DecoderParameters::ThreadType, data.threadType, FF_THREAD_SLICE, parseThreadType);
    parse(DecoderParameters::ThreadCount, data.threadCount, decoder::defaultThreadCount, decoder::parseThreadCount);
    
    // auto threadCountEntry = map.find(DecoderParameters::ThreadCount);
    // if(threadCountEntry != map.end())
    // {
    //     auto casted = std::stoi(threadCountEntry->second);
    //     casted = std::min(casted, maxThreadCount);
    //     casted = std::max(casted, minThreadCount);
    //     data.threadCount = casted;
    // }
    // else
    //     data.threadCount = defaultThreadCount;

    return data;
}
