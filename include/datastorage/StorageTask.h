#pragma once
#include "Timer.h"
#include "DataReceiver.h"
#include "BinaryParserConfigurator.h"

#include <fstream>

namespace StorageNS {
	class StorageTask: public TimerTask
	{
	public:
		StorageTask(DataReceiver &receiver)
			:receiver(receiver), valid_data_length(0)
		{}

		~StorageTask() {}

		int config(const char *config_file_path, const char *binary_file_path);

		void run() override;

	private:
		DataReceiver &receiver;
		std::unique_ptr<JsonConfigurator> configurator;
		SequencedParser parsers;
		int valid_data_length;
		std::ofstream binary_stream;
	};
}