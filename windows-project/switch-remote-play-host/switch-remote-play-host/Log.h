#pragma once

#include "UnicodeStringUtility.h"
#include "LogImportanceOptions.h"
#include <stdint.h>
#include <string>
#include <ostream>

class Log
{
public:
    Log(encodedOutputStream& stream, LogImportance logFilter, bool logToFile);

    void Write(std::string const str, LogImportance level);
    void Write(std::string const str, LogImportance level, bool timestamp);
    void Write(std::wstring const str, LogImportance level);
    void Write(std::wstring const str, LogImportance level, bool timestamp);

    Log& operator<<(uint64_t const num);
    Log& operator<<(std::string const& str);
    Log& operator<<(std::wstring const& str);
    Log& operator<<(LogImportance const level);

private:
    encodedOutputStream& outputStream;
    encodedOutputStream dummyStream;
    encodedOutputFile fileStream;
    LogImportance minimumLevel;
    LogImportance streamLevel;
};
