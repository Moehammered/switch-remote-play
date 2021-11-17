#pragma once

#include "UnicodeStringUtility.h"
#include <string>
#include <ostream>

enum LogImportance
{
    Verbose,
    Low,
    Medium,
    High
};

class Log
{
public:
    Log(encodedOutputStream& stream, LogImportance logFilter);

    void Write(std::string const str, LogImportance level);
    void Write(std::string const str, LogImportance level, bool timestamp);
    void Write(std::wstring const str, LogImportance level);
    void Write(std::wstring const str, LogImportance level, bool timestamp);

    encodedOutputStream& operator<<(std::string const& str);
    encodedOutputStream& operator<<(std::wstring const & str);
    encodedOutputStream& operator<<(LogImportance const level);

private:
    encodedOutputStream& outputStream;
    encodedOutputStream dummyStream;
    LogImportance minimumLevel;
    LogImportance streamLevel;
};
