#include "WSAHelper.h"
#include <WinSock2.h>

std::string WSAErrorToStr(int code)
{
    switch (code)
    {
    case WSANOTINITIALISED:
        return "WSANOTINITIALISED - A successful WSAStartup call must occur before using this function.";

    case WSAENETDOWN:
        return "WSAENETDOWN - The network subsystem has failed.";

    case WSAEFAULT:
        return "WSAEFAULT - The buf parameter is not completely contained in a valid part of the user address space.";

    case WSAENOTCONN:
        return "WSAENOTCONN - The socket is not connected.";

    case WSAEINTR:
        return "WSAEINTR - The (blocking) call was cancelled through WSACancelBlockingCall";

    case WSAEINPROGRESS:
        return "WSAEINPROGRESS - A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.";

    case WSAENETRESET:
        return "WSAENETRESET - For a connection-oriented socket, this error indicates that the connection has been broken due to keep-alive activity that detected a failure while the operation was in progress. For a datagram socket, this error indicates that the time to live has expired.";

    case WSAENOTSOCK:
        return "WSAENOTSOCK - The descriptor is not a socket.";

    case WSAEOPNOTSUPP:
        return "WSAEOPNOTSUPP - MSG_OOB was specified, but the socket is not stream-style such as type SOCK_STREAM, OOB data is not supported in the communication domain associated with this socket, or the socket is unidirectional and supports only send operations.";

    case WSAESHUTDOWN:
        return "WSAESHUTDOWN - The socket has been shut down; it is not possible to receive on a socket after shutdown has been invoked with how set to SD_RECEIVE or SD_BOTH.";

    case WSAEWOULDBLOCK:
        return "WSAEWOULDBLOCK - The socket is marked as nonblocking and the receive operation would block.";

    case WSAEMSGSIZE:
        return "WSAEMSGSIZE - The message was too large to fit into the specified buffer and was truncated.";

    case WSAEINVAL:
        return "WSAEINVAL - The socket has not been bound with bind, or an unknown flag was specified, or MSG_OOB was specified for a socket with SO_OOBINLINE enabled or (for byte stream sockets only) len was zero or negative.";

    case WSAECONNABORTED:
        return "WSAECONNABORTED - The virtual circuit was terminated due to a time-out or other failure. The application should close the socket as it is no longer usable.";

    case WSAETIMEDOUT:
        return "WSAETIMEDOUT - The connection has been dropped because of a network failure or because the peer system failed to respond.";

    case WSAECONNRESET:
        return "WSAECONNRESET - The virtual circuit was reset by the remote side executing a hard or abortive close. The application should close the socket as it is no longer usable. On a UDP-datagram socket, this error would indicate that a previous send operation resulted in an ICMP \"Port Unreachable\" message.";

    default:
        return "Unknown error code: " + std::to_string(code);
    }
}