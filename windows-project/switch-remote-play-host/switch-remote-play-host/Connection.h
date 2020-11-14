#pragma once
#include <winsock2.h>
#include <stdint.h>
#include <string>

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

