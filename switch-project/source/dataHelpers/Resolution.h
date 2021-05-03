#ifndef __RESOLUTION_H__
#define __RESOLUTION_H__

#include <stdint.h>
#include <string>
#include <vector>

struct Resolution
{
    int16_t width;
    int16_t height;
};

inline bool operator==(Resolution const & a, Resolution const & b)
{
    return a.width == b.width && a.height == b.height;
}

inline bool operator!=(Resolution const & a, Resolution const & b)
{
    return !(a == b);
}

std::string ResolutionToString(Resolution const & res);
Resolution ParseResolutionString(std::string s);

static std::vector<Resolution> const DesktopResolutions
{
    Resolution{1024, 576}, Resolution{1152, 648}, 
    Resolution{1280, 720}, Resolution{1366, 768},
    Resolution{1600, 900}, Resolution{1920, 1080},
    Resolution{2560, 1440}
};

static std::vector<Resolution> const SwitchResolutions 
{ 
    Resolution{1280, 720}//, Resolution{1920, 1080}
};

static Resolution const hdRes{1280, 720};
static Resolution const fullHdRes{1920, 1080};

#endif
