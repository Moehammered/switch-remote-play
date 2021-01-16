//
//  VirtualMouse.cpp
//  helloworld
//
//  Created by Moehammered on 12/1/21.
//

#include "VirtualMouse.h"
#include <CoreGraphics/CGEvent.h>
#include <iostream>

VirtualMouse::VirtualMouse()
: leftDown{false}, rightDown{false}, position{}, lastPosition{},
leftClickFlag{DEFAULT_LEFT_CLICK_FLAG}, rightClickFlag{DEFAULT_RIGHT_CLICK_FLAG}, flags{0}
{
    CGEventRef ourEvent = CGEventCreate(NULL);
    auto point = CGEventGetLocation(ourEvent);
    CFRelease(ourEvent);
    
    position = point;
    lastPosition = point;
}

void VirtualMouse::SetLeftClickFlag(unsigned int flag)
{
    leftClickFlag = flag;
}

void VirtualMouse::SetRightClickFlag(unsigned int flag)
{
    rightClickFlag = flag;
}

void VirtualMouse::Move(int x, int y)
{
    position.x += x;
    position.y += y;
}

void VirtualMouse::SetButtons(unsigned int flags)
{
    this->flags = flags;
}

void VirtualMouse::ResetPosition()
{
    Move(-position.x, -position.y);
}

void VirtualMouse::ResetButtons()
{
    SetButtons(0);
}

void VirtualMouse::Reset()
{
    ResetPosition();
    ResetButtons();
}

void VirtualMouse::Update()
{
    auto xMoved = (lastPosition.x - position.x) != 0;
    auto yMoved =(lastPosition.y - position.y) != 0;
    
//    std::cout << "  mouse flag: << " << flags << "\n\n";
    if(leftDown)
    {
        if(flags & leftClickFlag)
        {
            // left being held
            if(xMoved || yMoved)
            {
                // left drag event
                auto moveEvent = CGEventCreateMouseEvent(nullptr, kCGEventLeftMouseDragged, position, kCGMouseButtonLeft);
                CGEventPost(kCGHIDEventTap, moveEvent);
                CFRelease(moveEvent);
                std::cout << "  dragging left mouse button\n\n";
            }
        }
        else
        {
            // left release
            auto release = CGEventCreateMouseEvent(nullptr, kCGEventLeftMouseUp, position, kCGMouseButtonLeft);
            CGEventPost(kCGHIDEventTap, release);
            CFRelease(release);
            leftDown = false;
            std::cout << "    releasing left mouse\n";
        }
    }
    else if(flags & leftClickFlag)
    {
        // left click
        auto click = CGEventCreateMouseEvent(nullptr, kCGEventLeftMouseDown, position, kCGMouseButtonLeft);
        CGEventPost(kCGHIDEventTap, click);
        CFRelease(click);
        leftDown = true;
        std::cout << "    left click\n";
    }
    else if(rightDown)
    {
        if(flags & rightClickFlag)
        {
            // right being held
            if(xMoved || yMoved)
            {
                // right drag event
                auto moveEvent = CGEventCreateMouseEvent(nullptr, kCGEventRightMouseDragged, position, kCGMouseButtonRight);
                CGEventPost(kCGHIDEventTap, moveEvent);
                CFRelease(moveEvent);
                std::cout << "  dragging right mouse button\n\n";
            }
        }
        else
        {
            // right release
            auto release = CGEventCreateMouseEvent(nullptr, kCGEventRightMouseUp, position, kCGMouseButtonRight);
            CGEventPost(kCGHIDEventTap, release);
            CFRelease(release);
            rightDown = false;
            std::cout << "    releasing right mouse\n";
        }
    }
    else if(flags & rightClickFlag)
    {
        // right click
        auto click = CGEventCreateMouseEvent(nullptr, kCGEventRightMouseDown, position, kCGMouseButtonRight);
        CGEventPost(kCGHIDEventTap, click);
        CFRelease(click);
        rightDown = true;
        std::cout << "    right click\n";
    }
    else if(xMoved || yMoved)
    {
        auto move = CGEventCreateMouseEvent(nullptr, kCGEventMouseMoved, position, kCGMouseButtonLeft);
        CGEventPost(kCGHIDEventTap, move);
        CFRelease(move);
        leftDown = false;
    }
    
    lastPosition = position;
}
