#pragma once

#include <winsock2.h>
#include <stdint.h>
#include <string>

class Broadcast
{
public:
	Broadcast(std::string subnet, uint16_t port);

	bool ReadyToSend();
	bool ReadyToRecv();

	template<typename T>
	bool Send(T data);

	bool Send(char const * const data, int size);

	bool Recv(std::string& data);

	std::string const ReceivedIP() const;

	bool Close();

	bool Shutdown();

private:
	SOCKET broadcastSocket;
	SOCKET receiverSocket;
	sockaddr_in broadcastAddr;
	sockaddr_in receiveAddr;

	int const ADDR_SIZE = sizeof(broadcastAddr);
};

template<typename T>
inline bool Broadcast::Send(T data)
{
	auto dataPtr = (const char*)(&data);
	auto dataSize = sizeof(data);

	auto result = sendto(broadcastSocket, dataPtr, dataSize, 0, 
		&broadcastAddr, ADDR_SIZE);

	return result != SOCKET_ERROR;
}

template<>
inline bool Broadcast::Send(std::string data)
{
	auto result = sendto(broadcastSocket, data.c_str(), data.length(), 0,
		(const sockaddr*)&broadcastAddr, ADDR_SIZE);

	return result != SOCKET_ERROR;
}