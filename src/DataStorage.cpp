#include <iostream>
#include <string>

#include "DataReceiver.h"
#include "TCPReceiver.h"
#include "UDPReceiver.h"
#include "StorageTask.h"
#include "StorageConverter.h"

using namespace StorageNS;

void testDataReceive()
{
	std::string transport_type = "UDP";
	DataReceiver &receiver = UDPReceiver(10240);

	if (transport_type == "TCP")
	{
		TCPReceiver *tcp_receiver = dynamic_cast<TCPReceiver*>(&receiver);

		while (!tcp_receiver->isConnected()) {
			tcp_receiver->accept();
		}
	}

	StorageTask task(receiver);
	task.config("type.json", "type.dat");

	Timer timer(task);
	StopWatch watcher;

	timer.start();
	watcher.start();

	int run_time = 0;
	while (true)
	{
		if (watcher.elapsed_s() > 60)
		{
			++run_time;
			watcher.restart();
			std::cout << "This program has been running for " << run_time << " minutes." << std::endl;
		}
	}
}

void testDataConvert()
{
	FormatSpecifier& specifier = FormatSpecifier::instance();
	specifier.set_specifier(FormatSpecifier::Type::FLOAT, "%.2f");
	specifier.set_specifier(FormatSpecifier::Type::DOUBLE, "%.2f");

	CsvStorageConverter converter = CsvStorageConverter();

	converter.setBinarySource("type.dat");
	converter.setTargetFile("type.csv");
	converter.setTemplate("type.json");

	if (converter.prepare() == -1)
	{
		return ;
	}

	std::cout << "Total item: " << converter.totalItem() << std::endl;

	size_t current_item = 0;

	StopWatch watcher;
	watcher.start();
	converter.storeHeaders();
	while(converter.hasNext()) {
		converter.convertAndStore();

		current_item = converter.currentItem();
		if (current_item % 100000 == 0) {
		    std::cout << "Processed Item: " << current_item << std::endl;
		}
	}
	watcher.stop();
	std::cout << "Processing time: " << watcher.elapsed_s() / 60 << " min" << std::endl;
}

int main(int argc, char *argv[])
{
	testDataConvert();
	//testDataReceive();

	return 0;
}
