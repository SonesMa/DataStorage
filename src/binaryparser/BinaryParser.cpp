#include "BinaryParser.h"

using namespace StorageNS;

ParserFactory::~ParserFactory()
{
	for (auto parser = parsers.begin(); parser != parsers.end(); ++parser)
	{
		if (parser->second != nullptr)
			delete parser->second;
	}
}

BinaryParser *ParserFactory::getParser(const std::string &type)
{
	auto parser_item = parsers.find(type);

	if (parser_item == parsers.end())
	{
		return nullptr;
	}

	return parser_item->second;
}

void ParserFactory::addParser(const std::string &type, BinaryParser *parser)
{
	if (parsers.find(type) == parsers.end())
	{
		parsers[type] = parser;
	}
}

struct ArrayTypeParser
{
	ArrayTypeParser(const std::string &type)
	{
		element_type = type.substr(0, type.find('['));
		std::string length_str = type.substr(type.find('[') + 1, type.find(']'));
		size = std::stoi(length_str);
	}

	std::string element_type;
	int size;
};

BinaryParser *ParserFactory::getArrayParser(const std::string &type)
{
	BinaryParser* parser = nullptr;

	if (parsers.find(type) == parsers.end())
	{
		ArrayTypeParser checker(type);
		parser = generateArrayParser(checker.element_type, checker.size);
	}
	else {
		parser = parsers[type];
	}

	return parser;
}

BinaryParser *ParserFactory::generateArrayParser(const std::string &element_type, int size)
{
	BinaryParser *parser = nullptr;

	if (element_type == "int8_t")
	{
		parser = new ArrayParser<int8_t>(size);
	}
	else if (element_type == "int16_t")
	{
		parser = new ArrayParser<int16_t>(size);
	}
	else if (element_type == "int32_t")
	{
		parser = new ArrayParser<int32_t>(size);
	}
	else if (element_type == "int64_t")
	{
		parser = new ArrayParser<int64_t>(size);
	}
	else if (element_type == "uint8_t")
	{
		parser = new ArrayParser<uint8_t>(size);
	}
	else if (element_type == "uint16_t")
	{
		parser = new ArrayParser<uint16_t>(size);
	}
	else if (element_type == "uint32_t")
	{
		parser = new ArrayParser<uint32_t>(size);
	}
	else if (element_type == "uint64_t")
	{
		parser = new ArrayParser<uint64_t>(size);
	}
	else if (element_type == "float")
	{
		parser = new ArrayParser<float>(size);
	}
	else if (element_type == "double")
	{
		parser = new ArrayParser<double>(size);
	}

	return parser;
}

bool ParserFactory::isArrayType(const std::string &type)
{
	if (type.find('[') != std::string::npos && type.find(']') != std::string::npos)
	{
		return true;
	}

	return false;
}

bool ParserFactory::isStructType(const std::string &type)
{
	return type == "struct";
}

int SequencedParser::fprintf(FILE* fp, const char* buffer)
{
	if (buffer == nullptr)
		throw NullBufferException();

	int offset = 0;
	FormatSpecifier &specifier = FormatSpecifier::instance();

	for (int i = 0; i < parsers.size(); ++i)
	{
		parsers[i].second->fprintf(fp, buffer + offset);
		offset += static_cast<int>(parsers[i].second->length());
		if (i != parsers.size() - 1)
		{
			std::fprintf(fp, "%s", specifier.get_delimiter());
		}
	}

	return offset;
}

size_t SequencedParser::length()
{
	return _length;
}

std::string SequencedParser::parse(const char *buffer)
{
	if (buffer == nullptr)
		throw NullBufferException();

	std::vector<MemberInfo> members;

	size_t offset = 0;
	std::string expr;

	for (auto iterator = parsers.begin(); iterator != parsers.end(); ++iterator)
	{
		auto parser = iterator->second;
		const char *buf = buffer + offset;
		expr += parser->parse(buf);
		offset += parser->length();
	}

	return expr;
}

std::vector<MemberInfo> SequencedParser::expr(const std::string &name, const char *buffer)
{
	std::vector<MemberInfo> members;

	if (buffer == nullptr)
		return members;

	size_t offset = 0;
	std::string expr_name;

	for (auto parser = parsers.begin(); parser != parsers.end(); ++parser)
	{
		if (name.empty()) {
			expr_name = parser->first;
		}
		else {
			expr_name = name + "." + parser->first;
		}
		auto sub_members = parser->second->expr(expr_name, buffer + offset);
		members.insert(members.end(), sub_members.begin(), sub_members.end());
		offset += parser->second->length();
	}

	return members;
}

std::vector<MemberInfo> SequencedParser::expr(const char* buffer)
{
	return expr("", buffer);
}

void SequencedParser::addParser(const std::string &name, BinaryParser *parser)
{
	parsers.emplace_back(std::make_pair(name, parser));
	_length += parser->length();
}

std::string StringParser::parse(const char *buffer)
{
	std::string expr(buffer);
	_length = expr.size();
	return expr;
}

std::vector<MemberInfo> StringParser::expr(const std::string &name, const char *buffer)
{
	std::string expr(buffer);
	std::vector<MemberInfo> members;

	if (buffer == nullptr)
		return members;

	members.emplace_back(MemberInfo(name, expr));

	_length = expr.size();

	return members;
}

size_t StringParser::length()
{
	return _length;
}