//
//  DisplayData.h
//  helloworld
//
//  Created by Moehammered on 10/1/21.
//

#ifndef DisplayData_h
#define DisplayData_h

#include <stdio.h>
#include <CoreGraphics/CGDisplayConfiguration.h>
#include <CoreGraphics/CGDirectDisplay.h>
#include <vector>


struct DisplayAttribute
{
    CGDisplayModeRef modeRef;
    size_t width;
    size_t height;
};

DisplayAttribute GetCurrentDisplayAttributes();

std::vector<DisplayAttribute> GetAllDisplayModes();

void PrintDisplayAttributes(std::vector<DisplayAttribute> const & attribs);

void SetDisplayMode(DisplayAttribute const & attrib);

#endif
