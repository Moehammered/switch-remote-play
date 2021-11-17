#include "Log.h"

Log::Log(encodedOutputStream& stream, LogImportance logFilter)
    : outputStream{ stream }, dummyStream{ nullptr },
    minimumLevel{ logFilter }, streamLevel{ LogImportance::Verbose }
{
}

void Log::Write(std::string const str, LogImportance level)
{
    Write(str, level, false);
}

void Log::Write(std::string const str, LogImportance level, bool timestamp)
{
    if (level >= minimumLevel)
    {
        if (timestamp)
            outputStream << "timestamp here\n";

        outputStream << transformString(str);
    }
}

void Log::Write(std::wstring const str, LogImportance level)
{
    Write(str, level, false);
}

void Log::Write(std::wstring const str, LogImportance level, bool timestamp)
{
    if (level >= minimumLevel)
    {
        if (timestamp)
            outputStream << "timestamp here\n";

        outputStream << transformString(str);
    }
}

encodedOutputStream& Log::operator<<(std::string const& str)
{
    if (streamLevel >= minimumLevel)
    {
        auto transformed = transformString(str);
        outputStream << transformed;
        return outputStream;
    }
    else
        return dummyStream;
}

encodedOutputStream& Log::operator<<(std::wstring const& str)
{
    if (streamLevel >= minimumLevel)
    {
        outputStream << transformString(str);
        return outputStream;
    }
    else
        return dummyStream;
}

encodedOutputStream& Log::operator<<(LogImportance const level)
{
    streamLevel = level;
    if (streamLevel >= minimumLevel)
        return outputStream;
    else
        return dummyStream;
}