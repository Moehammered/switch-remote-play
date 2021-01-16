#include "MasterVolume.h"
#include <iostream>

MasterVolume::MasterVolume()
	: masterVolumeEndpoint{ nullptr }
{
    auto init = CoInitialize(NULL);

    switch (init)
    {
        case S_OK:
        case S_FALSE:
            {
                auto audioDevice = DefaultAudioDevice();
                if (audioDevice != nullptr)
                {
                    masterVolumeEndpoint = GetVolumeEndpoint(audioDevice);
                    audioDevice->Release();
                    audioDevice = nullptr;
                }
            }
            break;

        default:
            std::cout << "CoInitialize failed or is not ready. Cannot enumerate audio devices.\n";
            break;
    }

    CoUninitialize();
}

MasterVolume::~MasterVolume()
{
    if (masterVolumeEndpoint != nullptr)
    {
        masterVolumeEndpoint->Release();
        masterVolumeEndpoint = nullptr;
    }
}

void MasterVolume::SetVolume(float percentage)
{
    if (masterVolumeEndpoint != nullptr)
        masterVolumeEndpoint->SetMasterVolumeLevelScalar(percentage, nullptr);
}

float MasterVolume::GetVolume()
{
    if (masterVolumeEndpoint != nullptr)
    {
        float vol{ -1 };
        auto res = masterVolumeEndpoint->GetMasterVolumeLevelScalar(&vol);
        if (res == S_OK)
            return vol;
        else
            return -1;
    }
    else
        return -1;
}

bool MasterVolume::IsMuted()
{
    if (masterVolumeEndpoint != nullptr)
    {
        BOOL muted{};
        auto res = masterVolumeEndpoint->GetMute(&muted);
        if (res == S_OK)
            return muted == 1;
        else
            return false;
    }
    else
        return false;
}

void MasterVolume::Mute(bool mute)
{
    if (masterVolumeEndpoint != nullptr)
        masterVolumeEndpoint->SetMute(mute, nullptr);
}

IMMDevice* MasterVolume::DefaultAudioDevice()
{
    HRESULT hr{ 0 };

    IMMDeviceEnumerator* deviceEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID*)&deviceEnumerator);
    switch (hr)
    {
        case S_OK:
            //std::cout << "An instance of the specified object class was successfully created\n";
            break;
        case REGDB_E_CLASSNOTREG:
            std::cout << "A specified class is not registered in the registration database. Also can indicate that the type of server you requested in the CLSCTX enumeration is not registered or the values for the server types in the registry are corrupt\n";
            return nullptr;
        case CLASS_E_NOAGGREGATION:
            std::cout << "This class cannot be created as part of an aggregate\n";
            return nullptr;
        case E_NOINTERFACE:
            std::cout << "The specified class does not implement the requested interface, or the controlling IUnknown does not expose the requested interface\n";
            return nullptr;
        case E_POINTER:
            std::cout << "The ppv parameter is NULL\n";
            return nullptr;
    }

    IMMDevice* defaultDevice = NULL;

    hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
    deviceEnumerator->Release();
    deviceEnumerator = NULL;

    if (hr == S_OK)
    {
        return defaultDevice;
    }
    else
    {
        switch (hr)
        {
            case E_INVALIDARG:
                std::cout << "Parameter dataFlow or role is out of range\n";
                break;
            case E_NOTFOUND:
                std::cout << "No device is available\n";
                break;
            case E_OUTOFMEMORY:
                std::cout << "Out of memory\n";
                break;
            case E_POINTER:
                std::cout << "The ppv parameter is NULL\n";
                break;
        }

        return nullptr;
    }
}

IAudioEndpointVolume* MasterVolume::GetVolumeEndpoint(IMMDevice* device)
{
    if (device != nullptr)
    {
        IAudioEndpointVolume* endpointVolume = NULL;
        auto hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID*)&endpointVolume);
        if (hr == S_OK)
            return endpointVolume;
        else
        {
            switch (hr)
            {
                case E_NOINTERFACE:
                    std::cout << "The object does not support the requested interface type\n";
                break;

                case E_POINTER:
                    std::cout << "Parameter ppInterface is NULL\n";
                break;

                case E_INVALIDARG:
                    std::cout << "The pActivationParams parameter must be NULL for the specified interface; or pActivationParams points to invalid data\n";
                break;

                case E_OUTOFMEMORY:
                    std::cout << "Out of memory\n";
                break;
            }

            return nullptr;
        }
    }
    else
        return nullptr;
}
