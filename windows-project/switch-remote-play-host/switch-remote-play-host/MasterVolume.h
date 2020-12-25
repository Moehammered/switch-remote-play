#pragma once

#include <mmdeviceapi.h>
#include <endpointvolume.h>

class MasterVolume
{
public:
	MasterVolume();
	~MasterVolume();

	void SetVolume(float percentage);
	float GetVolume();

	bool IsMuted();
	void Mute(bool mute);

private:
	IAudioEndpointVolume* masterVolumeEndpoint;

	IMMDevice* DefaultAudioDevice();
	IAudioEndpointVolume* GetVolumeEndpoint(IMMDevice* device);
};

