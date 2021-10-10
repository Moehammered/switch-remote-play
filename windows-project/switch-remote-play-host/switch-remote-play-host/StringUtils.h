#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

inline std::string Replace(std::string const& a, char target, char replacement)
{
	auto copy = std::string{ a };
	for (auto& c : copy)
		if (c == target)
			c = replacement;
	return copy;
}

inline std::string TrimLeft(std::string const& a)
{
	if (a.size() == 0)
		return a;
	else
	{
		auto firstNonSpace = size_t{0};
		auto isWhiteSpace = [](char const& a)
		{
			return a == ' ' || a == '\t';
		};

		while (isWhiteSpace(a[firstNonSpace]) && firstNonSpace < a.size())
			++firstNonSpace;

		return a.substr(firstNonSpace);
	}
}

inline std::string TrimRight(std::string const& a)
{
	if (a.size() == 0)
		return a;
	else
	{
		auto firstNonSpace = a.size() - 1;
		auto isWhiteSpace = [](char const& a)
		{
			return a == ' ' || a == '\t';
		};

		while (isWhiteSpace(a[firstNonSpace]) && firstNonSpace > 0)
			--firstNonSpace;

		return a.substr(0, firstNonSpace + 1);
	}
}

inline std::string Trim(std::string const& a)
{
	return TrimRight(TrimLeft(a));
}

inline std::string ToLower(std::string const& a)
{
	auto copy = std::string{ a };

	for (auto& c : copy)
		c = std::tolower(c);

	return copy;
}

inline std::string ToUpper(std::string const& a)
{
	auto copy = std::string{ a };

	for (auto& c : copy)
		c = std::toupper(c);

	return copy;
}

inline std::vector<std::string> SplitLines(std::string const& data)
{
	auto stream = std::stringstream(data);
	auto lines = std::vector<std::string>{};
	
	auto line = std::string{};
	while (std::getline(stream, line, '\n'))
		lines.emplace_back(line);

	return lines;
}