#ifndef __WORDDELIMITER_H__
#define __WORDDELIMITER_H__

#include <string>
#include <vector>

/*
    Simple word delimiter implementation
    https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/

    Great website for CPP brainstorming and articles.
*/
std::vector<std::string> split(const std::string& s, char delimiter);

#endif
