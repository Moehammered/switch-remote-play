#include "NetworkAdapter.h"
#include <iostream>
#include <iomanip>

std::vector<std::string> split(std::string const token, std::string const& s)
{
    auto lastPos = 0;
    auto counter = 0;

    std::vector<std::string> splits{};

    while (counter < s.size())
    {
        auto substr = s.substr(counter, token.size());
        if (substr == token)
        {
            splits.emplace_back(s.substr(lastPos, counter - lastPos));
            counter += token.size();
            lastPos = counter;
        }
        else
            ++counter;
    }
    splits.emplace_back(s.substr(lastPos, counter - lastPos));

    return splits;
}

std::string CreateBroadcastAddress(std::string const gateway, std::string const subnetMask)
{
    auto gatewaySegments = split(".", gateway);
    auto subnetSegments = split(".", subnetMask);
    auto broadcastAddr = std::string{};

    for (auto i = 0; i < gatewaySegments.size(); ++i)
    {
        auto a = std::stoul(gatewaySegments[i]);
        auto b = std::stoul(subnetSegments[i]);
        auto c = (a | (~b)) & 0xff;
        broadcastAddr += std::to_string(c) + ".";
    }

    return broadcastAddr.substr(0, broadcastAddr.size() - 1);
}

bool ScanNetworkConnections(std::string& broadcastAddress, Log& logger)
{
    NetworkAdapter a{};
    auto activeAdapters = a.ActiveAdapters();

    if (activeAdapters.size() >= 1)
    {
        if (activeAdapters.size() > 1)
        {
            logger << "Found '" << activeAdapters.size() << "' active network connections... Displaying now\n\n";
            for (auto const& a : activeAdapters)
                PrintSimpleAdapterInfo(a, logger);

            logger << "\n\n Assuming the main active network is the first one.\n";
        }
        auto const& adapter = activeAdapters[0];
        PrintSimpleAdapterInfo(adapter, logger);
        if (adapter.gatewayAddresses.size() > 0 && adapter.subnetMasks.size() > 0)
        {
            auto gateway = std::string(adapter.gatewayAddresses[0].begin(), adapter.gatewayAddresses[0].end());
            auto broadcastAddr = CreateBroadcastAddress(gateway, adapter.subnetMasks[0]);
            logger << "    Broadcast:   " << transformString(broadcastAddr) << "\n\n";
            broadcastAddress = broadcastAddr;
        }
        return true;
    }
    else
    {
        logger << LogImportance::High;
        logger << "!!! No active network connections found !!!\n\n";
        logger << "Please make sure your PC is connected to a network.\n";
        logger << "Also make sure your Switch is connected to the same network!\n\n";

        logger << "Assuming default broadcast address: " << transformString(broadcastAddress) << "\n\n";

        logger << "If you cannot connect, please check your network connection and try restarting the application\n\n";

        return false;
    }
}

bool ScanNetworkConnections(std::string& broadcastAddress)
{
    NetworkAdapter a{};
    auto activeAdapters = a.ActiveAdapters();

    if (activeAdapters.size() >= 1)
    {
        auto const& adapter = activeAdapters[0];
        if (adapter.gatewayAddresses.size() > 0 && adapter.subnetMasks.size() > 0)
        {
            auto gateway = std::string(adapter.gatewayAddresses[0].begin(), adapter.gatewayAddresses[0].end());
            auto broadcastAddr = CreateBroadcastAddress(gateway, adapter.subnetMasks[0]);
            broadcastAddress = broadcastAddr;
        }
        return true;
    }
    else
        return false;
}

void PrintAdapterName(AdapterInfo const& adapter, Log& logger)
{
    logger << adapter.friendlyName << " ";
    logger << adapter.name << "\n";
    logger << "    " << adapter.description << "\n";
}

void PrintInterfaceInfo(AdapterInfo const& adapter, Log& logger)
{
    auto interfaceType = transformString(GetInterfaceType(adapter.interfaceType));
    auto status = transformString(GetStatus(adapter.status));
    logger << "    Connection type:                " << interfaceType << "\n";
    logger << "    Status:                         " << status << "\n";
}

void PrintSimpleAdapterInfo(AdapterInfo const& adapter, Log& logger)
{
    PrintAdapterName(adapter, logger);
    PrintInterfaceInfo(adapter, logger);
    auto tspeed_mbits = adapter.transmitSpeed / 1000 / 1000;
    auto tspeed_mbytes = adapter.transmitSpeed / 8.0 / 1024.0 / 1024.0;
    logger << "    Max Transmission Speed:         " << tspeed_mbits << " megabits/sec | " << tspeed_mbytes << " megabytes/sec\n";
    auto rspeed_mbits = adapter.transmitSpeed / 1000 / 1000;
    auto rspeed_mbytes = adapter.transmitSpeed / 8.0 / 1024.0 / 1024.0;
    logger << "    Max Receive Speed:              " << rspeed_mbits << " megabits/sec | " << rspeed_mbytes << " megabytes/sec\n";
    if (adapter.unicastAddresses.size() > 0)
        logger << "\n    IP Address:  " << transformString(adapter.unicastAddresses[0]) << "\n";
    if (adapter.gatewayAddresses.size() > 0)
        logger << "    Gateway:     " << transformString(adapter.gatewayAddresses[0]) << "\n";
    if (adapter.subnetMasks.size() > 0)
        logger << "    Subnet Mask: " << transformString(adapter.subnetMasks[0]) << "\n";
}

std::vector<std::string> GetAdapterFlagsInfo(uint64_t flags)
{
    std::vector<std::string> info{};

    if (flags & IP_ADAPTER_DDNS_ENABLED)
        info.emplace_back("Dynamic DNS enabled");

    if (flags & IP_ADAPTER_REGISTER_ADAPTER_SUFFIX)
        info.emplace_back("DNS suffix registered");

    if (flags & IP_ADAPTER_DHCP_ENABLED)
        info.emplace_back("DHCP enabled");

    if (flags & IP_ADAPTER_RECEIVE_ONLY)
        info.emplace_back("Receive only");

    if (flags & IP_ADAPTER_NO_MULTICAST)
        info.emplace_back("No multicast");

    if (flags & IP_ADAPTER_IPV6_OTHER_STATEFUL_CONFIG)
        info.emplace_back("IPv6 state info available");

    if (flags & IP_ADAPTER_NETBIOS_OVER_TCPIP_ENABLED)
        info.emplace_back("NetBIOS over TCP/IP");

    if (flags & IP_ADAPTER_IPV4_ENABLED)
        info.emplace_back("IPv4 enabled");

    if (flags & IP_ADAPTER_IPV6_ENABLED)
        info.emplace_back("IPv6 enabled");

    if (flags & IP_ADAPTER_IPV6_MANAGE_ADDRESS_CONFIG)
        info.emplace_back("IPv6 managed address enabled");

    return info;
}

std::string GetInterfaceType(uint64_t type)
{
    switch (type)
    {
    case IF_TYPE_ETHERNET_CSMACD:
        return "Ethernet interface adapter";

    case IF_TYPE_ISO88025_TOKENRING:
        return "Token Ring interface adapter";

    case IF_TYPE_PPP:
        return "PPP interface adapter";

    case IF_TYPE_SOFTWARE_LOOPBACK:
        return "Loopback interface adapter";

    case IF_TYPE_ATM:
        return "ATM interface adapter";

    case IF_TYPE_IEEE80211:
        return "802.11(Wireless) interface adapter";

    case IF_TYPE_TUNNEL:
        return "Tunnel interface adapter";

    case IF_TYPE_IEEE1394:
        return "Firewire interface adapter";

    case IF_TYPE_OTHER:
    default:
        return "Unknown interface adapter";
    }
}

std::string GetStatus(uint64_t status)
{
    switch (status)
    {
    case IfOperStatusUp:
        return "Active";

    case IfOperStatusDown:
        return "Inactive";

    case IfOperStatusTesting:
        return "Testing mode";

    case IfOperStatusDormant:
        return "Dormant";

    case IfOperStatusNotPresent:
        return "Unavailable";

    case IfOperStatusLowerLayerDown:
        return "Inactive (due to depent interface adapter being inactive)";

    case IfOperStatusUnknown:
    default:
        return "unknown";
    }
}

NetworkAdapter::NetworkAdapter()
{
    auto dummyLogStream = encodedOutputStream{ nullptr };
    auto dummyLog = Log(dummyLogStream, LogImportance::High);
    adapters = FindAdapterInfo(dummyLog);
    FindAdapterSubnetMasks(adapters, dummyLog);
}

NetworkAdapter::NetworkAdapter(Log& logger)
{
    adapters = FindAdapterInfo(logger);
    FindAdapterSubnetMasks(adapters, logger);
}

std::vector<AdapterInfo> const NetworkAdapter::ActiveAdapters()
{
    std::vector<AdapterInfo> active{};

    for (auto const& a : adapters)
    {
        if (IsActive(a.status) && a.interfaceType != IF_TYPE_SOFTWARE_LOOPBACK && !a.gatewayAddresses.empty())
            active.push_back(a);
    }

    return active;
}

void NetworkAdapter::PrintActiveAdaptersInfo(Log& logger)
{
    for (auto const& a : adapters)
    {
        if (!IsActive(a.status) || a.interfaceType == IF_TYPE_SOFTWARE_LOOPBACK || a.gatewayAddresses.empty())
            continue;

        logger << "\n\n";
        PrintAdapterName(a, logger);
        PrintInterfaceInfo(a, logger);

        logger << "    Unicast Addresses\n";
        for (auto const& u : a.unicastAddresses)
            logger << "        " << u << "\n";
        logger << "\n";

        logger << "    Gateway Addresses\n";
        for (auto const& u : a.gatewayAddresses)
            logger << "        " << u << "\n";
        logger << "\n";

        auto mtuKB = a.maxTransmissionUnit / 1024.0;
        logger << "    Max Transmission Unit :         " << a.maxTransmissionUnit << " bytes | " << mtuKB << " kilobytes\n";
        auto tspeed_mbits = a.transmitSpeed / 1000 / 1000;
        auto tspeed_mbytes = a.transmitSpeed / 8.0 / 1024.0 / 1024.0;
        logger << "    Transmission Speed:             " << tspeed_mbits << " megabits/sec | " << tspeed_mbytes << " megabytes/sec\n";
        auto rspeed_mbits = a.transmitSpeed / 1000 / 1000;
        auto rspeed_mbytes = a.transmitSpeed / 8.0 / 1024.0 / 1024.0;
        logger << "    Receive Speed:                  " << rspeed_mbits << " megabits/sec | " << rspeed_mbytes << " megabytes/sec\n";

        logger << "\n";
        logger << "    Adapter Flags\n";
        auto flags = GetAdapterFlagsInfo(a.flags);
        for (auto const& f : flags)
            logger << "        " << transformString(f) << "\n";
        logger << "\n\n";
    }
}

std::vector<AdapterInfo> NetworkAdapter::FindAdapterInfo(Log& logger)
{
    std::vector<AdapterInfo> adapters{};

    DWORD dwRetVal = 0;
    auto constexpr WORKING_BUFFER_SIZE = 15000;
    auto constexpr MAX_TRIES = 3;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS;

    // default to unspecified address family (both)
    ULONG family = AF_INET;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG iterations = 0;

    // Allocate a 15 KB buffer to start with.
    outBufLen = WORKING_BUFFER_SIZE;
    do
    {
        pAddresses = new IP_ADAPTER_ADDRESSES[outBufLen];
        if (pAddresses == NULL)
        {
            logger.Write("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n", LogImportance::High, true);
            return adapters;
        }

        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW)
        {
            delete[] pAddresses;
            pAddresses = NULL;
        }
        else
            break;

        iterations++;
    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR)
    {
        // If successful, output some information from the data we received
        auto pCurrAddresses = pAddresses;

        while (pCurrAddresses)
        {
            AdapterInfo adapter{};
            adapter.name = std::string{ pCurrAddresses->AdapterName };
            adapter.description = std::wstring{ pCurrAddresses->Description };
            adapter.friendlyName = std::wstring{ pCurrAddresses->FriendlyName };
            adapter.transmitSpeed = pCurrAddresses->TransmitLinkSpeed;
            adapter.receiveSpeed = pCurrAddresses->ReceiveLinkSpeed;
            adapter.flags = pCurrAddresses->Flags;
            adapter.maxTransmissionUnit = pCurrAddresses->Mtu;
            adapter.interfaceType = pCurrAddresses->IfType;
            adapter.status = pCurrAddresses->OperStatus;

            {
                auto pUnicast = pCurrAddresses->FirstUnicastAddress;
                std::vector<std::wstring> unicastAddresses{};
                if (pUnicast != nullptr)
                {
                    for (auto i = 0; pUnicast != nullptr; i++)
                    {
                        WCHAR buff[512]{ 0 };
                        DWORD size = 512;
                        WSAAddressToString(pUnicast->Address.lpSockaddr, pUnicast->Address.iSockaddrLength, nullptr, buff, &size);

                        unicastAddresses.emplace_back(buff);
                        pUnicast = pUnicast->Next;
                    }
                }
                adapter.unicastAddresses = unicastAddresses;
            }
            {
                auto pAnycast = pCurrAddresses->FirstAnycastAddress;
                std::vector<std::wstring> anycastAddresses{};
                if (pAnycast != nullptr)
                {
                    for (auto i = 0; pAnycast != nullptr; i++)
                    {
                        WCHAR buff[512]{ 0 };
                        DWORD size = 512;
                        WSAAddressToString(pAnycast->Address.lpSockaddr, pAnycast->Address.iSockaddrLength, nullptr, buff, &size);
                        anycastAddresses.emplace_back(buff);
                        pAnycast = pAnycast->Next;
                    }
                }
                adapter.anycastAddresses = anycastAddresses;
            }
            {
                auto pMulticast = pCurrAddresses->FirstMulticastAddress;
                std::vector<std::wstring> multicastAddresses{};
                if (pMulticast != nullptr)
                {
                    for (auto i = 0; pMulticast != nullptr; i++)
                    {
                        WCHAR buff[512]{ 0 };
                        DWORD size = 512;
                        WSAAddressToString(pMulticast->Address.lpSockaddr, pMulticast->Address.iSockaddrLength, nullptr, buff, &size);
                        multicastAddresses.emplace_back(buff);
                        pMulticast = pMulticast->Next;
                    }
                }
                adapter.multicastAddresses = multicastAddresses;
            }
            {
                auto pGateway = pCurrAddresses->FirstGatewayAddress;
                std::vector<std::wstring> gatewayAddresses{};
                if (pGateway != nullptr)
                {
                    for (auto i = 0; pGateway != nullptr; i++)
                    {
                        WCHAR buff[512]{ 0 };
                        DWORD size = 512;
                        WSAAddressToString(pGateway->Address.lpSockaddr, pGateway->Address.iSockaddrLength, nullptr, buff, &size);
                        gatewayAddresses.emplace_back(buff);
                        pGateway = pGateway->Next;
                    }
                }
                adapter.gatewayAddresses = gatewayAddresses;
            }
            {
                auto pDnServer = pCurrAddresses->FirstDnsServerAddress;
                std::vector<std::wstring> dnsAddresses{};
                if (pDnServer != nullptr)
                {
                    for (auto i = 0; pDnServer != nullptr; i++)
                    {
                        WCHAR buff[512]{ 0 };
                        DWORD size = 512;
                        WSAAddressToString(pDnServer->Address.lpSockaddr, pDnServer->Address.iSockaddrLength, nullptr, buff, &size);
                        dnsAddresses.emplace_back(buff);
                        pDnServer = pDnServer->Next;
                    }
                }
                adapter.dnsAddresses = dnsAddresses;
            }

            adapter.dnsSuffix = std::wstring{ pCurrAddresses->DnsSuffix };

            std::vector<uint16_t> physicalAddress{};
            for (auto i = 0; i < (int)pCurrAddresses->PhysicalAddressLength; i++)
                physicalAddress.push_back(pCurrAddresses->PhysicalAddress[i]);
            adapter.physicalAddress = physicalAddress;

            pCurrAddresses = pCurrAddresses->Next;
            adapters.push_back(adapter);
        }
    }
    else
    {
        logger << LogImportance::High;
        logger << "Call to GetAdaptersAddresses failed with error: " << dwRetVal << "\n";
        if (dwRetVal == ERROR_NO_DATA)
            logger.Write("\tNo addresses were found for the requested parameters\n", LogImportance::High, true);
        else
        {
            LPVOID lpMsgBuf = NULL;
            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                // Default language
                (LPTSTR)&lpMsgBuf, 0, NULL)) 
            {
                logger << "\tError: " << (LPSTR)lpMsgBuf << "\n";
                LocalFree(lpMsgBuf);
                if (pAddresses)
                {
                    delete[] pAddresses;
                    pAddresses = nullptr;
                }
            }
        }
    }

    if (pAddresses != nullptr)
        delete[] pAddresses;

    return adapters;
}

void NetworkAdapter::FindAdapterSubnetMasks(std::vector<AdapterInfo>& adapters, Log& logger)
{
    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    auto pAdapterInfo = new IP_ADAPTER_INFO[ulOutBufLen];
    if (pAdapterInfo == NULL)
    {
        logger.Write("Error allocating memory needed to call GetAdaptersinfo\n", LogImportance::High, true);
        return;
    }

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        delete[] pAdapterInfo;
        pAdapterInfo = nullptr;
        pAdapterInfo = new IP_ADAPTER_INFO[ulOutBufLen];
        if (pAdapterInfo == nullptr)
        {
            logger.Write("Error allocating memory needed to call GetAdaptersinfo\n", LogImportance::High, true);
            return;
        }
    }

    auto dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
    if (dwRetVal == NO_ERROR)
    {
        auto pAdapter = pAdapterInfo;
        while (pAdapter)
        {
            for (auto& adapter : adapters)
            {
                if (pAdapter->AddressLength == adapter.physicalAddress.size())
                {
                    auto match = true;
                    for (auto i = 0; i < pAdapter->AddressLength; ++i)
                    {
                        if (pAdapter->Address[i] != adapter.physicalAddress[i])
                        {
                            match = false;
                            break;
                        }
                    }
                    if (match)
                    {
                        PIP_ADDR_STRING addr = &(pAdapter->IpAddressList);
                        while (addr)
                        {
                            adapter.subnetMasks.emplace_back(addr->IpMask.String);
                            addr = addr->Next;
                        }
                        break;
                    }
                }
            }
            pAdapter = pAdapter->Next;
        }
    }
    else
    {
        logger << LogImportance::High;
        logger << "GetAdaptersInfo failed with error: " << dwRetVal << "\n";
    }

    if (pAdapterInfo)
    {
        delete[] pAdapterInfo;
        pAdapterInfo = nullptr;
    }
}

bool NetworkAdapter::IsActive(uint64_t status)
{
    return status == IfOperStatusUp;
}
