#include "StorageTask.h"

#include <iostream>

using namespace StorageNS;

int StorageTask::config(const char *config_file, const char *binary_file)
{
	std::string content = StorageNS::getTextFileContent("type.json");

	configurator = std::unique_ptr<JsonConfigurator>(new JsonConfigurator(content));

	if (!configurator->isValid()){
		return -1;
	}
	parsers = configurator->generateParser();
	valid_data_length = static_cast<int>(parsers.length());

	binary_stream.open(binary_file, std::ios::binary|std::ios::out|std::ios::trunc);

	return 0;
}

void StorageTask::run()
{
	buffer_t buffer = receiver.receive(valid_data_length);
	if (buffer.length == valid_data_length)
	{
		binary_stream.write(buffer.data.get(), buffer.length);
	}
}