//
//  VirtualMouse.hpp
//  helloworld
//
//  Created by Moehammered on 12/1/21.
//

#ifndef VirtualMouse_hpp
#define VirtualMouse_hpp
#include <CoreGraphics/CGEventTypes.h>
#include <stdio.h>

unsigned int constexpr DEFAULT_LEFT_CLICK_FLAG = 1;
unsigned int constexpr DEFAULT_RIGHT_CLICK_FLAG = 2;

class VirtualMouse
{
public:
    VirtualMouse();
    
    void SetLeftClickFlag(unsigned int flag);
    void SetRightClickFlag(unsigned int flag);
    void Move(int x, int y);
    void SetButtons(unsigned int flags);
    void Update();
    void ResetPosition();
    void ResetButtons();
    void Reset();
    
private:
    bool leftDown;
    bool rightDown;
    CGPoint position;
    CGPoint lastPosition;
    
    unsigned int leftClickFlag;
    unsigned int rightClickFlag;
    unsigned int flags;
};

#endif /* VirtualMouse_hpp */
