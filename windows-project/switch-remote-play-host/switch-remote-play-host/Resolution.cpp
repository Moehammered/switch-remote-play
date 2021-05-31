#include "Resolution.h"
#include <iostream>

std::string ResolutionToString(Resolution const& res)
{
    return std::to_string(res.width) + "x" + std::to_string(res.height);
}

Resolution ParseResolutionString(std::string s)
{
    auto widthStart = 0;
    auto widthEnd = s.find('x', 0);
    auto heightStart = widthEnd + 1;
    auto heightEnd = s.length();

    if (widthEnd == std::string::npos || heightEnd == std::string::npos)
    {
        std::cout << "failed to find delimiters for " << s << ": " << widthEnd << ", " << heightEnd << std::endl;
        return { 1280, 720 };
    }
    else
    {
        auto width = s.substr(widthStart, widthEnd - widthStart);
        auto height = s.substr(heightStart, heightEnd - heightStart);

        Resolution temp{};

        temp.width = atoi(width.c_str());
        temp.height = atoi(height.c_str());

        return temp;
    }
}