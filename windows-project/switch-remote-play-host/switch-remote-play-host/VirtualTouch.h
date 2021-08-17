#pragma once
#include <Windows.h>
#include <WinUser.h>
#include <vector>
#include <unordered_map>

struct VirtualFinger
{
    uint32_t id;
    uint32_t x;
    uint32_t y;
};

class VirtualTouch
{
public:
    VirtualTouch(int deadzoneRadius, int radius);

    void Press();
    void Move(int x, int y);
    void Press(std::vector<VirtualFinger> const fingers);
    void Move(std::vector<VirtualFinger> const fingers);
    void Release();

    void Commit();

private:
    int const deadzoneRadius;
    int const deadzoneMagSqr;
    int const radius;
    std::unordered_map<uint32_t, POINTER_TOUCH_INFO> contacts;
    std::unordered_map<uint32_t, POINTER_TOUCH_INFO> released;

    bool OutsideDeadzone(int x1, int y1, int x2, int y2);

    POINTER_TOUCH_INFO CreateContact(uint32_t id);
    void TrackMissing(std::vector<VirtualFinger> const& fingers);

    void PressContact(POINTER_TOUCH_INFO& contact);
    void MoveContact(POINTER_TOUCH_INFO& contact, int x, int y);
    void ReleaseContact(POINTER_TOUCH_INFO& contact);

    std::vector<POINTER_TOUCH_INFO> CreateReleaseEvents(std::vector<POINTER_TOUCH_INFO> const& contacts);
};