#pragma once

#include <memory>

namespace StorageNS {
	/**
	* This class is used as a buffer container which will delete internal buffer
	* automatically when destruction.
	*/
	struct buffer_t {
		buffer_t()
			:length(0) {}

		buffer_t(char *data, size_t size)
			:data(data), length(size) {}

		buffer_t(buffer_t &&other)
		{
			data = std::move(other.data);
			length = other.length;
		}

		buffer_t &operator=(buffer_t &&other)
		{
			data = std::move(other.data);
			length = other.length;
		}

		std::unique_ptr<char[]> data;
		size_t length;
	};

	/**
	* This class acts as a base class for data receiver supporting receive 
	* fixed-size data.
	*/
	class DataReceiver {
	public:
		virtual ~DataReceiver() {};

		/**
		* @brief  Receive data with given fixed-size
		* @param  size [in] - the size of data to receive
		* @return 
		*         buffer_t - encapsulated buffer with size contained.
		*/
		virtual buffer_t receive(int size) = 0;
	};
}