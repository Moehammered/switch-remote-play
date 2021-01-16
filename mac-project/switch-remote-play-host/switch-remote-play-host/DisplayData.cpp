
//
//  DisplayData.cpp
//  helloworld
//
//  Created by Moehammered on 10/1/21.
//

#include "DisplayData.h"
#include <iostream>
#include <algorithm>

DisplayAttribute GetCurrentDisplayAttributes()
{
    DisplayAttribute attrib{};
    
    auto mode = CGDisplayCopyDisplayMode(CGMainDisplayID());
    attrib.width = CGDisplayModeGetPixelWidth(mode);
    attrib.height = CGDisplayModeGetPixelHeight(mode);
    attrib.modeRef = mode;
    
    return attrib;
}

std::vector<DisplayAttribute> GetAllDisplayModes()
{
    std::vector<DisplayAttribute> attribs;
    
    auto modes = CGDisplayCopyAllDisplayModes(CGMainDisplayID(), nullptr);
    
    auto count = CFArrayGetCount(modes);
    
    for(auto i = 0U; i < count; ++i)
    {
        auto mode = (CGDisplayModeRef)CFArrayGetValueAtIndex(modes, i);
        auto w = CGDisplayModeGetPixelWidth(mode);
        auto h = CGDisplayModeGetPixelHeight(mode);
        attribs.emplace_back(DisplayAttribute{
                                    .modeRef = mode,
                                    .width = w,
                                    .height = h
                                });
    }
    
    CFRelease(modes);
    
    return attribs;
}

void SetDisplayMode(DisplayAttribute const & attrib)
{
    auto modes = GetAllDisplayModes();
    auto match = std::find_if(modes.begin(), modes.end(), [&] (DisplayAttribute const & b){
        return attrib.height == b.height && attrib.width == b.width;
    });
    if(match != modes.end())
        CGDisplaySetDisplayMode (CGMainDisplayID(), match->modeRef, NULL); // 9
}

void PrintDisplayAttributes(std::vector<DisplayAttribute> const & attribs)
{
    for(auto & a : attribs)
        std::cout << "Mode -- " << a.modeRef << " -- " << a.width << " x " << a.height << "\n";
    std::cout << "\n";
}
