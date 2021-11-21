#include "DateTimeUtility.h"
#include <sstream>
#include <ctime>

std::string padLeft(std::string const str, size_t const size, char const paddingCharacter = ' ')
{
    if (str.size() > size)
        return str;
    else
    {
        auto copy = std::string{ str };
        copy.insert(0, size - str.size(), paddingCharacter);
        return copy;
    }
}

DateTime ToDateTime(tm& ctimeStruct)
{
    auto dateTime = DateTime{};

    dateTime.years = 1900 + ctimeStruct.tm_year;
    dateTime.months = 1 + ctimeStruct.tm_mon;
    dateTime.days = ctimeStruct.tm_mday;
    dateTime.hours = ctimeStruct.tm_hour;
    dateTime.minutes = ctimeStruct.tm_min;
    dateTime.seconds = ctimeStruct.tm_sec;

    dateTime.dayOfTheYear = ctimeStruct.tm_yday;
    dateTime.dayOfTheWeek = (Weekday)ctimeStruct.tm_wday;

    return dateTime;
}

DateTime LocalDateTime()
{
    auto const now = time(0);
    auto localTime = tm{};
    localtime_s(&localTime, &now);

    return ToDateTime(localTime);
}

DateTime UTCDateTime()
{
    auto const now = time(0);
    auto utctime = tm{};
    gmtime_s(&utctime, &now);

    return ToDateTime(utctime);
}

std::string DateToFileName(DateTime const dt)
{
    auto ss = std::stringstream{ };

    auto const years = std::to_string(dt.years);
    auto const months = padLeft(std::to_string(dt.months), 2, '0');
    auto const days = padLeft(std::to_string(dt.days), 2, '0');
    auto const hours = padLeft(std::to_string(dt.hours), 2, '0');
    auto const minutes = padLeft(std::to_string(dt.minutes), 2, '0');
    auto const seconds = padLeft(std::to_string(dt.seconds), 2, '0');

    ss << years << "." << months << "." << days;
    ss << "_" << hours << "-" << minutes << "-" << seconds;

    return ss.str();
}
