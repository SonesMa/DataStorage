#include "BinaryParserConfigurator.h"
#include "BinaryParser.h"

#include <iostream>

using namespace ArduinoJson;
using namespace StorageNS;

JsonConfigurator::JsonConfigurator( const std::string &json_string )
	: _doc(json_string.size() * 2)
{
	DeserializationError error = deserializeJson(_doc, json_string);

	if (error)
	{
		_valid = false;
		return ;
	}

	_valid = true;

	_factory.addParser("int8_t", new BasicParser<int8_t>());
	_factory.addParser("int16_t", new BasicParser<int16_t>());
    _factory.addParser("int32_t", new BasicParser<int32_t>());
	_factory.addParser("int64_t", new BasicParser<int64_t>());
	_factory.addParser("uint8_t", new BasicParser<uint8_t>());
	_factory.addParser("uint16_t", new BasicParser<uint16_t>());
	_factory.addParser("uint32_t", new BasicParser<uint32_t>());
	_factory.addParser("uint64_t", new BasicParser<uint64_t>());
	_factory.addParser("float", new BasicParser<float>());
	_factory.addParser("double", new BasicParser<double>());
}

bool JsonConfigurator::isValid()
{
	return _valid;
}

SequencedParser JsonConfigurator::generateParser()
{
	JsonArray descriptions = _doc["TypeDescription"];
	SequencedParser parsers;

	for (size_t i = 0; i < descriptions.size(); ++i) {
		ArduinoJson::JsonObject obj = descriptions[i];
		const char* name = obj.getMember("name");
		const char* type = obj.getMember("type");
		const char* custom_type = obj.getMember("concreteType");

		auto parser = generateParser(name, type, custom_type);

		parsers.addParser(name, parser);
	}

	return parsers;
}

BinaryParser * JsonConfigurator::generateParser( const char *name, const char *type, const char*concreteType )
{
	const char* key = type;
	
	if (_factory.isStructType(type)) {
		key = concreteType;
	}

	BinaryParser *parser = _factory.getParser(key);

	if (parser == nullptr) {
		if (_factory.isArrayType(type))
		{
			parser = composeArrayParser(key);
		}
		else if (_factory.isStructType(type))
		{
			parser = composeStructParser(key);
		}
		_factory.addParser(key, parser);
	}

	return parser;
}

BinaryParser* JsonConfigurator::composeParser(const char *key)
{
	JsonArray descriptions = _doc[key];
	SequencedParser *parsers = new SequencedParser();

	for (size_t i = 0; i < descriptions.size(); ++i) {
		ArduinoJson::JsonObject obj = descriptions[i];
		const char* name = obj.getMember("name");
		const char* type = obj.getMember("type");
		const char* custom_type = obj.getMember("concreteType");

		auto parser = generateParser(name, type, custom_type);

		parsers->addParser(name, parser);
	}

	return parsers;
}

BinaryParser* JsonConfigurator::composeStructParser(const char *key)
{
	JsonArray descriptions = _doc[key];
	SequencedParser *parsers = new SequencedParser();

	for (size_t i = 0; i < descriptions.size(); ++i) {
		ArduinoJson::JsonObject obj = descriptions[i];
		const char* name = obj.getMember("name");
		const char* type = obj.getMember("type");
		const char* custom_type = obj.getMember("concreteType");

		auto parser = generateParser(name, type, custom_type);

		parsers->addParser(name, parser);
	}

	return parsers;
}

BinaryParser* JsonConfigurator::composeArrayParser(const char *key)
{
	auto parser = _factory.getArrayParser(key);

	return parser;
}
