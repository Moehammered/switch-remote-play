#pragma once
#include <vector>
#include <string>

struct DisplayDeviceInfo
{
    int32_t index;
    std::wstring monitorID;
    std::wstring monitorKey;
    std::wstring monitorSystemName;
    std::wstring monitorName;
    std::wstring deviceName;
    std::wstring interfaceAdapter;
    std::wstring interfaceAdapterKey;
    uint64_t deviceStateFlags;
    uint64_t monitorStateFlags;
};

void PrintDisplayDeviceInfo(DisplayDeviceInfo const display);

class DisplayDeviceService
{
public:
    DisplayDeviceService();
    DisplayDeviceService(bool activeOnly);

    void PrintDisplays() const;
    std::vector<DisplayDeviceInfo> const ActiveDisplay() const;
    DisplayDeviceInfo const PrimaryDisplay() const;

private:
    std::vector<DisplayDeviceInfo> const devices;

    std::vector<DisplayDeviceInfo> QueryDevices(bool activeOnly);
};