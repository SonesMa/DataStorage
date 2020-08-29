#pragma once

#ifdef WIN32
#pragma comment(lib,"Ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>

typedef SOCKET socket_t;

static inline int socket_init()
{
	WSADATA wsaData = {0};
    return WSAStartup(MAKEWORD(2, 2), &wsaData);
}

static inline int socket_destroy()
{
	return WSACleanup();
}

#elif defined(_POSIX_C_SOURCE)
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

typedef int socket_t;
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1

#define closesocket(s) close(s)
static inline int socket_init()
{
	return 0;
}

static inline int socket_destroy()
{
	return 0;
}
#endif
