#include "NetworkAdapter.h"
#include <iostream>
#include <iomanip>

NetworkAdapter::NetworkAdapter()
{
    adapters = FindAdapterInfo();
}

void NetworkAdapter::PrintAdaptersInfo()
{
    using namespace std;
    for (auto const& a : adapters)
    {
        if (!IsActive(a.status) || a.interfaceType == IF_TYPE_SOFTWARE_LOOPBACK || a.gatewayAddresses.empty())
            continue;

        cout << "\n\n";
        wcout << a.friendlyName << " ";
        cout << a.name << "\n";
        wcout << "    " << a.description << "\n";
        cout << "    Connection type:                " << GetInterfaceType(a.interfaceType) << "\n";
        cout << "    Status:                         " << GetStatus(a.status) << "\n";
        
        cout << "    Unicast Addresses\n";
        for (auto const& u : a.unicastAddresses)
            wcout << "        " << u << "\n";
        cout << "\n";

        cout << "    Gateway Addresses\n";
        for (auto const& u : a.gatewayAddresses)
            wcout << "        " << u << "\n";
        cout << "\n";

        auto mtuKB = a.maxTransmissionUnit / 1024.0;
        wcout << "    Max Transmission Unit :         " << a.maxTransmissionUnit << " bytes | " << mtuKB << " kilobytes\n";
        auto tspeed_mbits = a.transmitSpeed / 1000 / 1000;
        auto tspeed_mbytes = a.transmitSpeed / 8.0 / 1024.0 / 1024.0;
        wcout << "    Transmission Speed:             " << tspeed_mbits << " megabits/sec | " << tspeed_mbytes << " megabytes/sec\n";
        auto rspeed_mbits = a.transmitSpeed / 1000 / 1000;
        auto rspeed_mbytes = a.transmitSpeed / 8.0 / 1024.0 / 1024.0;
        wcout << "    Receive Speed:                  " << rspeed_mbits << " megabits/sec | " << rspeed_mbytes << " megabytes/sec\n";

        cout << "\n";
        cout << "    Adapter Flags\n";
        auto flags = GetAdapterFlagsInfo(a.flags);
        for (auto const& f : flags)
            cout << "        " << f << "\n";
        cout << "\n\n";
    }
}

std::vector<AdapterInfo> NetworkAdapter::FindAdapterInfo()
{
    std::vector<AdapterInfo> adapters{};

    DWORD dwSize = 0;
    DWORD dwRetVal = 0;
    auto constexpr WORKING_BUFFER_SIZE = 15000;
    auto constexpr MAX_TRIES = 3;

    // Set the flags to pass to GetAdaptersAddresses
    ULONG flags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_INCLUDE_GATEWAYS;

    // default to unspecified address family (both)
    ULONG family = AF_INET;

    LPVOID lpMsgBuf = NULL;

    PIP_ADAPTER_ADDRESSES pAddresses = NULL;
    ULONG outBufLen = 0;
    ULONG Iterations = 0;

    // Allocate a 15 KB buffer to start with.
    outBufLen = WORKING_BUFFER_SIZE;

    do {

        pAddresses = new IP_ADAPTER_ADDRESSES[outBufLen];
        if (pAddresses == NULL) {
            printf("Memory allocation failed for IP_ADAPTER_ADDRESSES struct\n");
            return adapters;
        }

        dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

        if (dwRetVal == ERROR_BUFFER_OVERFLOW) {
            delete[] pAddresses;
            pAddresses = NULL;
        }
        else
            break;

        Iterations++;

    } while ((dwRetVal == ERROR_BUFFER_OVERFLOW) && (Iterations < MAX_TRIES));

    if (dwRetVal == NO_ERROR) {
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
    else {
        printf("Call to GetAdaptersAddresses failed with error: %d\n", dwRetVal);
        if (dwRetVal == ERROR_NO_DATA)
            printf("\tNo addresses were found for the requested parameters\n");
        else 
        {
            if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                // Default language
                (LPTSTR)&lpMsgBuf, 0, NULL)) {
                printf("\tError: %s", (LPSTR)lpMsgBuf);
                LocalFree(lpMsgBuf);
                if (pAddresses)
                {
                    delete[] pAddresses;
                    pAddresses = nullptr;
                }
            }
        }
    }

    if (pAddresses != nullptr) {
        delete[] pAddresses;
    }

    return adapters;
}

std::vector<std::string> NetworkAdapter::GetAdapterFlagsInfo(uint64_t flags)
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

std::string NetworkAdapter::GetInterfaceType(uint64_t type)
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

std::string NetworkAdapter::GetStatus(uint64_t status)
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

bool NetworkAdapter::IsActive(uint64_t status)
{
    return status == IfOperStatusUp;
}
