#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <stdint.h>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>

class Connection
{
public:
	Connection(uint16_t port);

	bool Ready() const;

	bool WaitForConnection();

	bool ConnectTo(std::string ip);

	SOCKET const& ConnectedSocket() const;
	SOCKET const& ListeningSocket() const;

	std::string const ConnectedIP() const;

	bool Disconnect();

	bool Close();

	bool Shutdown();

private:
	SOCKET listeningSocket;
	SOCKET connectedSocket;
	sockaddr_in serverAddr;
	sockaddr_in clientAddr;
	uint16_t portNo;
};

#endif
