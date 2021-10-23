#pragma once
#include <Windows.h>
#include <WinUser.h>
#include <vector>
#include <unordered_map>
#include "VirtualFinger.h"
#include "TouchInterface.h"

class VirtualTouch : public TouchInterface
{
public:
    VirtualTouch(int deadzoneRadius, int radius);

    void Update(std::vector<VirtualFinger> const fingers) override;
    void Release() override;

    void Commit() override;

private:
    int const deadzoneRadius;
    long const deadzoneMagSqr;
    int const radius;
    std::unordered_map<uint32_t, POINTER_TOUCH_INFO> contacts;
    std::unordered_map<uint32_t, POINTER_TOUCH_INFO> released;

    POINTER_TOUCH_INFO CreateContact(uint32_t id);
    void TrackMissing(std::vector<VirtualFinger> const& fingers);

    void PressContact(POINTER_TOUCH_INFO& contact);
    void MoveContact(POINTER_TOUCH_INFO& contact, int x, int y);
    void ReleaseContact(POINTER_TOUCH_INFO& contact);

    std::vector<POINTER_TOUCH_INFO> CreateReleaseEvents(std::vector<POINTER_TOUCH_INFO> const& contacts);
};