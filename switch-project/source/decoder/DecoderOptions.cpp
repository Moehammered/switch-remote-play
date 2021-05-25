#include "DecoderOptions.h"
#include "DecoderFlag.h"
#include "DecoderAccel.h"
#include "DiscardType.h"
#include "ThreadType.h"
#include <sstream>
#include "../utils/StringUtils.h"

std::unordered_map<DecoderParameters, std::string> DecoderParamsToStr(DecoderData const data)
{
    auto values = std::unordered_map<DecoderParameters, std::string>{};

    auto flags = DecoderFlag{};
    auto f1Values = flags.Flag1Strings(data.flags1);
    auto f2Values = flags.Flag2Strings(data.flags2);

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

    auto accFlags = DecoderAccel{};
    auto accValues = accFlags.ToStrings(data.accelFlags);
    values[DecoderParameters::AccelFlags] = csv(accValues);

    auto discFlags = DiscardType{};
    auto discVal = discFlags.ToString(data.discardFilter);
    values[DecoderParameters::DiscardFilter] = discVal;

    auto threadFlags = ThreadType{};
    auto typeVal = threadFlags.ToString(data.threadType);
    values[DecoderParameters::ThreadType] = typeVal;

    auto threadCount = std::to_string(data.threadCount);
    values[DecoderParameters::ThreadCount] = threadCount;

    return values;
}

DecoderData DecoderParamsFromStr(std::unordered_map<DecoderParameters, std::string> const & map)
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
        auto parser = DecoderFlag{};
        return parser.Flags1(flags);
    };

    auto parseFlags2 = [&](auto const & csv)
    {
        auto flags = csvToList(csv);
        auto parser = DecoderFlag{};
        return parser.Flags2(flags);
    };

    auto parseAccelFlags = [&](auto const & csv)
    {
        auto flags = csvToList(csv);
        auto parser = DecoderAccel{};
        return parser.Flags(flags);
    };

    auto parseDiscard = [](auto const & val)
    {
        auto dc = DiscardType{};
        return dc.Value(val);
    };

    auto parseThreadType = [](auto const & val)
    {
        auto tt = ThreadType{};
        return tt.Flag(val);
    };

    parse(DecoderParameters::Flags1, data.flags1, 0, parseFlags1);
    parse(DecoderParameters::Flags2, data.flags2, 0, parseFlags2);
    parse(DecoderParameters::AccelFlags, data.accelFlags, 0, parseAccelFlags);
    parse(DecoderParameters::DiscardFilter, data.discardFilter, AVDiscard::AVDISCARD_DEFAULT, parseDiscard);
    parse(DecoderParameters::ThreadType, data.threadType, FF_THREAD_SLICE, parseThreadType);
    
    auto threadCountEntry = map.find(DecoderParameters::ThreadCount);
    if(threadCountEntry != map.end())
    {
        auto casted = std::stoi(threadCountEntry->second);
        casted = std::min(casted, MaxThreadCount);
        casted = std::max(casted, MinThreadCount);
        data.threadCount = casted;
    }
    else
        data.threadCount = DefaultThreadCount;

    return data;
}
