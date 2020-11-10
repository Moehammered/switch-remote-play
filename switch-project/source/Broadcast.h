#ifndef __BROADCAST_H__
#define __BROADCAST_H__

#include <netinet/in.h>
#include <sys/socket.h>
#include <stdint.h>
#include <string>

class Broadcast
{
public:
	Broadcast(std::string subnet, uint16_t port);

	bool ReadyToSend();
	bool ReadyToRecv();

	template<class T>
	bool Send(T data);

	bool Send(char const * const data, int size);

	bool Recv(std::string& data);

	std::string const ReceivedIP() const;

	bool Close();

	bool Shutdown();
	
private:
	int broadcastSocket;
	int receiverSocket;
	sockaddr_in broadcastAddr;
	sockaddr_in receiveAddr;

	static socklen_t constexpr ADDR_SIZE = sizeof(broadcastAddr);
};

template<class T>
inline bool Broadcast::Send(T data)
{
	auto dataPtr = (const char*)(&data);
	auto dataSize = sizeof(data);

	auto result = sendto(broadcastSocket, dataPtr, dataSize, SO_BROADCAST, 
		(const sockaddr*)&broadcastAddr, ADDR_SIZE);

	return result >= 0;
}

template<>
inline bool Broadcast::Send(std::string data)
{
	auto result = sendto(broadcastSocket, data.c_str(), data.length(), SO_BROADCAST,
		(const sockaddr*)&broadcastAddr, ADDR_SIZE);

	return result >= 0;
}

#endif
