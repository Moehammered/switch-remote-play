#pragma once
#include "Log.h"
#include <vector>
#include <string>

struct DisplayDeviceInfo
{
    int32_t index;
    int64_t x;
    int64_t y;
    int64_t width;
    int64_t height;
    uint64_t deviceStateFlags;
    uint64_t monitorStateFlags;
    std::wstring monitorID;
    std::wstring monitorKey;
    std::wstring monitorSystemName;
    std::wstring monitorName;
    std::wstring deviceName;
    std::wstring interfaceAdapter;
    std::wstring interfaceAdapterKey;
};

void PrintDisplayDeviceInfo(DisplayDeviceInfo const display, Log& logger);

class DisplayDeviceService
{
public:
    DisplayDeviceService();
    DisplayDeviceService(bool activeOnly);

    void PrintDisplays(Log& logger) const;
    std::vector<DisplayDeviceInfo> const ActiveDisplays() const;
    DisplayDeviceInfo const PrimaryDisplay() const;

private:
    std::vector<DisplayDeviceInfo> const devices;

    std::vector<DisplayDeviceInfo> QueryDevices(bool activeOnly);
};