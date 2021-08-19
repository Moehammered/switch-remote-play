#include "DisplayDeviceService.h"
#include <Windows.h>
#include <WinUser.h>
#include <algorithm>
#include <iostream>

#include <sstream>
#include <unordered_map>
std::wstring DeviceStateFlagsToStr(DWORD flags)
{
    auto map = std::unordered_map<DWORD, std::wstring>{
        {DISPLAY_DEVICE_ACTIVE, L"disp_dev_active"},
        {DISPLAY_DEVICE_MIRRORING_DRIVER, L"disp_dev_mirror_driver"},
        {DISPLAY_DEVICE_MODESPRUNED, L"disp_dev_modes_pruned"},
        {DISPLAY_DEVICE_PRIMARY_DEVICE, L"disp_dev_primary"},
        {DISPLAY_DEVICE_REMOVABLE, L"disp_dev_removeable"},
        {DISPLAY_DEVICE_VGA_COMPATIBLE, L"disp_dev_vga_compat"},
        {DISPLAY_DEVICE_ATTACHED_TO_DESKTOP, L"disp_dev_attached_desktop"},
        {DISPLAY_DEVICE_MULTI_DRIVER, L"disp_dev_multi_driver"},
        {DISPLAY_DEVICE_ACC_DRIVER, L"disp_dev_acc_driver"},
        {DISPLAY_DEVICE_RDPUDD, L"disp_dev_rpdudd"},
        {DISPLAY_DEVICE_REMOTE, L"disp_dev_remote"},
        {DISPLAY_DEVICE_DISCONNECT, L"disp_dev_disconnect"},
        {DISPLAY_DEVICE_TS_COMPATIBLE, L"disp_dev_ts_compat"},
        {DISPLAY_DEVICE_UNSAFE_MODES_ON, L"disp_dev_unsafe_on"},
        {DISPLAY_DEVICE_ATTACHED, L"disp_dev_attached"}
    };

    auto ss = std::wstringstream{};

    for (auto const& entry : map)
    {
        if (entry.first & flags)
            ss << entry.second << " ";
    }

    return ss.str();
}

void PrintDisplayDeviceInfo(DisplayDeviceInfo const display)
{
    auto isPrimary = [&] {
        auto mask = display.deviceStateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE;
        return mask == DISPLAY_DEVICE_PRIMARY_DEVICE;
    };
    std::wcout << display.monitorName << "(" << display.deviceName << ")\n";
    std::wcout << "    Position: " << display.index;
    std::wcout << "    Primary: " << (isPrimary() ? "Yes" : "No") << "\n";
    std::wcout << "    Monitor ID: " << display.monitorID << "\n";
    std::wcout << "    Monitor Key: " << display.monitorKey << "\n";
    std::wcout << "    Monitor System Name: " << display.monitorSystemName << "\n\n";
    std::wcout << "    Display Adapter Info\n";
    std::wcout << "        Adapter: " << display.interfaceAdapter << "\n";
    std::wcout << "        Adapter Key: " << display.interfaceAdapterKey << "\n";
}

DisplayDeviceService::DisplayDeviceService()
    : devices{QueryDevices(true)}
{
}

DisplayDeviceService::DisplayDeviceService(bool activeOnly)
    : devices{ QueryDevices(activeOnly) }
{
}

void DisplayDeviceService::PrintDisplays() const
{
    if (devices.size() != 0)
    {
        for (auto const& device : devices)
        {
            std::wcout << "\n";
            PrintDisplayDeviceInfo(device);
            std::wcout << "\n";
        }
    }
    else
        std::wcout << "No devices found\n";
}

std::vector<DisplayDeviceInfo> const DisplayDeviceService::ActiveDisplay() const
{
    return devices;
}

DisplayDeviceInfo const DisplayDeviceService::PrimaryDisplay() const
{
    auto itr = std::find_if(devices.begin(), devices.end(), [](auto const& display) {
        return display.deviceStateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE;
    });

    if (itr != devices.end())
        return *itr;
    else
    {
        auto dummy = DisplayDeviceInfo{};
        dummy.monitorID = L"N/A";
        dummy.monitorKey = L"N/A";
        dummy.deviceName = L"N/A";
        dummy.monitorName = L"N/A";
        return dummy;
    }
}

std::vector<DisplayDeviceInfo> DisplayDeviceService::QueryDevices(bool activeOnly)
{
    auto systemDevices = std::vector<DISPLAY_DEVICE>{};
    auto count = 0;
    auto temp = DISPLAY_DEVICE{};
    ZeroMemory(&temp, sizeof(temp));
    temp.cb = sizeof(DISPLAY_DEVICE);

    while (EnumDisplayDevices(NULL, count, &temp, EDD_GET_DEVICE_INTERFACE_NAME))
    {
        if (!activeOnly)
            systemDevices.push_back(temp);
        else if (activeOnly && temp.StateFlags & DISPLAY_DEVICE_ACTIVE)
            systemDevices.push_back(temp);
        ++count;
    }

    auto displays = std::vector<DisplayDeviceInfo>{};

    for (auto const& device : systemDevices)
    {
        auto display = DisplayDeviceInfo{};
        display.index = displays.size();
        display.interfaceAdapter = device.DeviceString;
        display.interfaceAdapterKey = device.DeviceKey;
        display.deviceStateFlags = device.StateFlags;
        display.monitorSystemName = device.DeviceName;

        auto dataFetch = EnumDisplayDevices(device.DeviceName, 0, &temp, 0);
        if (dataFetch)
        {
            display.monitorID = temp.DeviceID;
            display.monitorKey = temp.DeviceKey;
            display.deviceName = temp.DeviceName;
            display.monitorName = temp.DeviceString;
            display.monitorStateFlags = temp.StateFlags;
        }
        else
        {
            display.deviceName = device.DeviceName;
            display.monitorID = L"N/A";
            display.monitorKey = L"N/A";
            display.monitorName = L"N/A";
        }

        displays.push_back(display);
    }

    return displays;
}
