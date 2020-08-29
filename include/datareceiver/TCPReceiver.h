#pragma once

#include "DataReceiver.h"
#include "os_socket.h"

namespace StorageNS {
	struct tcplink_t {
		socket_t local;
		socket_t accepted;
	};

	class TCPReceiver: public DataReceiver {
	public:
		/**
		* @brief  This construction function will create socket endpoint with
		*         given port. While the address of the socket is bind to INADDR_ANY
		*/
		TCPReceiver(unsigned short port);
		~TCPReceiver();

		/**
		* @brief  Receive data with given fixed-size
		* @param  size [in] - the size of data to receive
		* @return 
		*         buffer_t - encapsulated buffer with size contained.
		*/
		buffer_t receive(int size) override;

		/**
		* @brief  Test if this TCP endpoint has been connected from client.
		* @return
		*         true if connected, false or not.
		*/
		bool isConnected();

		/**
		* @brief  The TCP socket is configured to be non-blocking, so this function 
		*         will return immediately if not client is connecting this endpoint.
		* @return
		*         true if client has been connected, false or not.
		*/
		bool accept();
	private:
		tcplink_t link;
		bool is_connected;

		/**
		* @brief  Configure TCP NODELAY so that short packet can be send as soon as
		*         possible.
		*/
		void setTcpNodelay();

		/**
		* @brief  Configure given socket as non-blocking state.
		* @param  s [in] - socket to be configured.
		*/
		void setNonBlocking(socket_t s);
	};
}