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

	SOCKET const& ConnectedSocket() const;
	SOCKET const& ListeningSocket() const;

	std::string const ConnectedIP() const;

	bool Close();

private:
	SOCKET listeningSocket;
	SOCKET connectedSocket;
	sockaddr_in serverAddr;
	sockaddr_in clientAddr;
};

