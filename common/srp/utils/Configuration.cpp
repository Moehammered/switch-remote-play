#include "Configuration.h"
#include "StringUtils.h"
#include <fstream>
#include <iostream>
#include <sstream>

Configuration::Configuration(std::string const file)
	: filepath{ file },
	lines{ LoadFile(file) }, delimiter{ '=' }, endOfLine{ ';' }
{
}

bool Configuration::SaveConfigFile()
{
	auto filestream = std::fstream(filepath, std::ios::out);
	if (filestream.bad())
	{
		std::cout << "Failed to open file for saving " << filepath << "\n\n";
		filestream.close();
		return false;
	}
	else
	{
		for (auto& line : lines)
			filestream << stringutil::trim(line) << "\n";
		filestream.close();
		return true;
	}
}

bool Configuration::ExtractVariable(std::string const variable, std::string& result) const
{
	for (auto& line : lines)
	{
		auto loweredVariable = stringutil::toLower(variable);
		auto loweredLine = stringutil::toLower(line);
		auto location = loweredLine.find(loweredVariable + "=", 0);
		if (location != std::string::npos && location == 0)
		{
			auto start = line.find('=', location);
			auto end = line.find(';', location);
			if (start != std::string::npos && end != std::string::npos)
			{
				end--;
				result = line.substr(start+1, end - start);
				return true;
			}
		}
	}
	return false;
}

bool Configuration::ReplaceVariable(std::string const variable, std::string const value, std::string& result)
{
	for (auto& line : lines)
	{
		auto loweredVariable = stringutil::toLower(variable);
		auto loweredLine = stringutil::toLower(line);
		auto location = loweredLine.find(loweredVariable + "=", 0);
		if (location != std::string::npos && location == 0)
		{
			auto start = line.find('=', location);
			auto end = line.find(';', location);
			if (start != std::string::npos && end != std::string::npos)
			{
				end--;
				line.replace(start+1, end-start, value);
				return ExtractVariable(variable, result);
			}
		}
	}
	return false;
}

void Configuration::CreateVariable(std::string const variable, std::string const value)
{
	auto buffer = std::string{};
	if (!ReplaceVariable(variable, value, buffer))
		lines.emplace_back(variable + "=" + value + ";");
}

std::vector<std::string> Configuration::LoadFile(std::string const filepath) const
{
	auto filestream = std::fstream(filepath, std::ios::in);
	if (filestream.bad())
	{
		std::cout << "Failed to open file for loading " << filepath << "\n\n";
		filestream.close();
		return {};
	}
	else
	{
		std::stringstream data;
		data << filestream.rdbuf();
		filestream.close();

		//std::cout << data.str() << "\n";
		auto dataLines = stringutil::splitLines(data.str());
		auto trimmedLines = std::vector<std::string>{};
		for(auto const& line : dataLines)
			trimmedLines.push_back(stringutil::trim(line));

		return trimmedLines;
	}
}
