#pragma once
#include <string>
#include <vector>
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
bool ScanNetworkConnections(std::string& subnet);

void PrintAdapterName(AdapterInfo const & adapter);
void PrintInterfaceInfo(AdapterInfo const& adapter);
void PrintSimpleAdapterInfo(AdapterInfo const& adapter);
std::vector<std::string> GetAdapterFlagsInfo(uint64_t flags);
std::string GetInterfaceType(uint64_t type);
std::string GetStatus(uint64_t status);

class NetworkAdapter
{
public:
	NetworkAdapter();

	std::vector<AdapterInfo> const ActiveAdapters();
	void PrintActiveAdaptersInfo();
	
private:
	std::vector<AdapterInfo> adapters;
	std::vector<AdapterInfo> FindAdapterInfo();
	void FindAdapterSubnetMasks(std::vector<AdapterInfo>& adapters);

	bool IsActive(uint64_t status);
};

