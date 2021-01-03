#include "WordDelimiter.h"
#include <sstream>

/*
    Simple word delimiter implementation
    https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/

    Great website for CPP brainstorming and articles.
*/
std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}
