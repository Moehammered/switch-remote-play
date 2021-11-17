#pragma once
#include "Log.h"
#include <string>
#include <vector>
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <iphlpapi.h>

struct AdapterInfo
{
	std::string name;
	std::wstring friendlyName;
	std::wstring description;
	std::vector<std::wstring> unicastAddresses;
	std::vector<std::wstring> anycastAddresses;
	std::vector<std::wstring> multicastAddresses;
	std::vector<std::wstring> gatewayAddresses;
	std::vector<std::wstring> dnsAddresses;
	std::vector<std::string> subnetMasks;
	std::wstring dnsSuffix;
	std::vector<uint16_t> physicalAddress;
	uint64_t flags;
	uint64_t maxTransmissionUnit;
	uint64_t interfaceType;
	uint64_t status;
	uint64_t transmitSpeed;
	uint64_t receiveSpeed;
};

std::vector<std::string> split(std::string const token, std::string const& s);
std::string CreateBroadcastAddress(std::string const gateway, std::string const subnetMask);
bool ScanNetworkConnections(std::string& broadcastAddress, Log& logger);
bool ScanNetworkConnections(std::string& broadcastAddress);

void PrintAdapterName(AdapterInfo const & adapter, Log& logger);
void PrintInterfaceInfo(AdapterInfo const& adapter, Log& logger);
void PrintSimpleAdapterInfo(AdapterInfo const& adapter, Log& logger);
std::vector<std::string> GetAdapterFlagsInfo(uint64_t flags);
std::string GetInterfaceType(uint64_t type);
std::string GetStatus(uint64_t status);

class NetworkAdapter
{
public:
	NetworkAdapter();
	NetworkAdapter(Log& logger);

	std::vector<AdapterInfo> const ActiveAdapters();
	void PrintActiveAdaptersInfo(Log& logger);
	
private:
	std::vector<AdapterInfo> adapters;
	std::vector<AdapterInfo> FindAdapterInfo(Log& logger);
	void FindAdapterSubnetMasks(std::vector<AdapterInfo>& adapters, Log& logger);

	bool IsActive(uint64_t status);
};

