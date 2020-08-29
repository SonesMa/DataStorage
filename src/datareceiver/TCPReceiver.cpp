#include "TCPReceiver.h"

using namespace StorageNS;

void TCPReceiver::setTcpNodelay()
{
	char opt = 1;
	setsockopt(link.local, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(char));
}

void TCPReceiver::setNonBlocking(socket_t s)
{
	int iomode = 1;
	ioctlsocket(s, FIONBIO, (u_long *)&iomode);
}

bool TCPReceiver::isConnected()
{
	return is_connected;
}

bool TCPReceiver::accept()
{
	if (!is_connected)
	{
		link.accepted = ::accept(link.local, NULL, NULL);
		if (link.accepted != INVALID_SOCKET)
		{
			is_connected = true;
			setNonBlocking(link.accepted);
		}
	}

	return is_connected;
}

TCPReceiver::TCPReceiver(unsigned short port)
{
	struct sockaddr_in addr;
	is_connected = false;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;	
	addr.sin_port = htons(port);

	socket_init();
	link.local = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (link.local == INVALID_SOCKET)
	{
		return ;
	}

	setTcpNodelay();

	if(bind(link.local, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
		return ;
	}

	if (listen(link.local, 1) == 0) {
		setNonBlocking(link.local);
		link.accepted = ::accept(link.local, NULL, NULL);
		if (link.accepted != INVALID_SOCKET)
		{
			is_connected = true;
			setNonBlocking(link.accepted);
		}
	}
}

TCPReceiver::~TCPReceiver()
{
	closesocket(link.accepted);
	closesocket(link.local);
	socket_destroy();
}

buffer_t TCPReceiver::receive(int size)
{
	char *buffer = new char[size];
	int received = 0;

	while (received < static_cast<int>(size)) {
		received = recv(link.accepted, buffer + received, size - received, 0);
		if (received <= 0) {
			received = 0;
			break;
		}
	}

	return buffer_t(buffer, received);
}