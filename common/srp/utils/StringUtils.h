#ifndef __STRINGUTILS_H__
#define __STRINGUTILS_H__

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

namespace stringutil
{
	inline std::string replace(std::string const& a, char target, char replacement)
	{
		auto copy = std::string{ a };
		for (auto& c : copy)
			if (c == target)
				c = replacement;
		return copy;
	}

	inline std::string trimLeft(std::string const& a)
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

	inline std::string trimRight(std::string const& a)
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

	inline std::string trim(std::string const& a)
	{
		return trimRight(trimLeft(a));
	}

	inline std::string toLower(std::string const& a)
	{
		auto copy = std::string{ a };

		for (auto& c : copy)
			c = std::tolower(c);

		return copy;
	}

	inline std::string toUpper(std::string const& a)
	{
		auto copy = std::string{ a };

		for (auto& c : copy)
			c = std::toupper(c);

		return copy;
	}

	inline std::vector<std::string> splitLines(std::string const& data)
	{
		auto stream = std::stringstream(data);
		auto lines = std::vector<std::string>{};
		
		auto line = std::string{};
		while (std::getline(stream, line, '\n'))
			lines.emplace_back(line);

		return lines;
	}
}

#endif
