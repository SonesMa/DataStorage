#pragma once

#include "DataReceiver.h"
#include "os_socket.h"

namespace StorageNS {
	struct udplink_t {
		socket_t local;
	};

	class UDPReceiver: public DataReceiver {
	public:
		/**
		* @brief  This construction function will create socket endpoint with
		*         given port. While the address of the socket is bind to INADDR_ANY
		*/
		UDPReceiver(unsigned short port);
		~UDPReceiver();

		/**
		* @brief  Receive data with given fixed-size
		* @param  size [in] - the size of data to receive
		* @return 
		*         buffer_t - encapsulated buffer with size contained.
		*/
		buffer_t receive(int size) override;
	private:
		udplink_t link;

		/**
		* @brief  Configure this endpoint internal socket as non-blocking state.
		*/
		void setNonBlocking();
	};
}