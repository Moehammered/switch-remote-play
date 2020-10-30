#pragma once
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdint.h>
#include <string>

class Connection
{
public:
	Connection(uint16_t port);

	bool Ready() const;

	bool WaitForConnection();

	int const& ConnectedSocket() const;
	int const& ListeningSocket() const;

	std::string const ConnectedIP() const;

	bool Close();

	bool Shutdown();

private:
	int listeningSocket;
	int connectedSocket;
	sockaddr_in serverAddr;
	sockaddr_in clientAddr;
	uint16_t portNo;
};

