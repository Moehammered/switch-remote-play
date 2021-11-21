#pragma once

#include <stdint.h>
#include <string>

enum Weekday
{
    Sunday = 0,
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday = 6
};

struct DateTime
{
    uint32_t years;
    uint32_t months;
    uint32_t days;
    uint32_t hours;
    uint32_t minutes;
    uint32_t seconds;

    uint32_t dayOfTheYear;
    Weekday dayOfTheWeek;
};

DateTime LocalDateTime();
DateTime UTCDateTime();

std::string DateToFileName(DateTime const dt);
