#ifndef __RESOLUTION_H__
#define __RESOLUTION_H__

#include <stdint.h>
#include <string>

struct Resolution
{
    int16_t width;
    int16_t height;
};

std::string ResolutionToString(Resolution const & res);
Resolution ParseResolutionString(std::string s);

#endif
