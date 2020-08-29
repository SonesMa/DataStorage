#include "StorageConverter.h"
#include "DataReceiver.h"

using namespace StorageNS;

void StorageConverter::setBinarySource(const std::string &source_file)
{
	source = source_file;
}

void StorageConverter::setTargetFile(const std::string &target_file)
{
	target = target_file;
}

void StorageConverter::setTemplate(const std::string &template_file)
{
	template_ = template_file;
}

size_t StorageConverter::totalItem()
{
	return total_item;
}

size_t StorageConverter::currentItem()
{
	return current_item;
}

bool StorageConverter::hasNext()
{
	if (current_item != total_item)
		return true;

	return false;
}

int CsvStorageConverter::prepare()
{
	std::string content = StorageNS::getTextFileContent(template_.data());
	configurator = std::unique_ptr<JsonConfigurator>(new JsonConfigurator(content));

	if (!configurator->isValid()){
		return -1;
	}

	parsers = configurator->generateParser();
	item_length = parsers.length();

	csv_file = std::fopen(target.data(), "w+");

	source_stream.open(source, std::ios::binary|std::ios::in);
	if (!source_stream.is_open())
	{
		return -1;
	}

	total_item = calculateFileSize(source_stream) / item_length;

	return 0;
}

int CsvStorageConverter::convertAndStore()
{
	char *buf = new char[item_length];
	source_stream.read(buf, item_length);

	parsers.fprintf(csv_file, buf);
	std::fprintf(csv_file, "\n");

	++current_item;

	return 0;
}

int CsvStorageConverter::storeHeaders()
{
	FormatSpecifier& specifier = FormatSpecifier::instance();
	auto member_infos = parsers.expr("");
	int result = 0;

	for (int i = 0; i < member_infos.size(); ++i)
	{
		result = std::fprintf(csv_file, "%s", member_infos[i].first.data());
		if (i != member_infos.size() - 1)
		{
			std::fprintf(csv_file, "%s", specifier.get_delimiter());
		}
	}
	std::fprintf(csv_file, "\n");

	return 0;
}
