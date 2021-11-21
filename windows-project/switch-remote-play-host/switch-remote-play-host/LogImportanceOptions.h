#pragma once

#include <unordered_map>
#include <string>

enum LogImportance
{
    Verbose,
    Low,
    Medium,
    High
};

static std::unordered_map<LogImportance, std::string> const LogImportanceDesc
{
    { LogImportance::Verbose, "Verbose" },
    { LogImportance::Low, "Low"},
    { LogImportance::Medium, "Medium"},
    { LogImportance::High, "High"}
};

std::string LogImportanceToStr(LogImportance const val);
LogImportance LogImportanceFromStr(std::string const str);
