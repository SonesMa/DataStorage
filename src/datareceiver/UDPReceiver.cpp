#include "UDPReceiver.h"

using namespace StorageNS;

void UDPReceiver::setNonBlocking()
{
	int iomode = 1;
	ioctlsocket(link.local, FIONBIO, (u_long *)&iomode);
}

UDPReceiver::UDPReceiver(unsigned short port)
{
	struct sockaddr_in addr;

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	socket_init();
	link.local = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (bind(link.local, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) 
	{
		return ;
	}

	setNonBlocking();
}

UDPReceiver::~UDPReceiver()
{
	closesocket(link.local);
	socket_destroy();
}

buffer_t UDPReceiver::receive(int size)
{
	char *buffer = new char[size];
	int received = 0;

	while (received < static_cast<int>(size)) {
		received = recvfrom(link.local, buffer + received, size - received, 0, NULL, NULL);
		if (received <= 0) {
			received = 0;
			break;
		}
	}

	return buffer_t(buffer, received);
}
