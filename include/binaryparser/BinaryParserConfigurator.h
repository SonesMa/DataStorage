//=============================================================================
/**
* @file    BinaryParserConfigurator.h
* @version v0.1
* @brief   Binary buffer Parser Configurator defines interfaces to generate 
*          Binary buffer Parser according to configuration files. Currently only
*          support configuration from JSON file.
*/
//=============================================================================
#pragma once
#include <string>

#include "BinaryParser.h"

#include "ArduinoJson.hpp"

namespace StorageNS {
	/**
	* This class defines the interface of binary parser configurator.
	*/
	class BinaryParserConfigurator {
	public:
		/**
		* @brief   Generate sequenced binary parser. This function should construct
		*          sequenced binary parser following specific configuration.
		* @returns
		*          SequencedParser.
		**/
		virtual SequencedParser generateParser() = 0;

		/**
		* @brief   Indicating that this configurator is valid or not.
		* @returns
		*          bool, true if it is valid, or false if not.
		*/
		virtual bool isValid() = 0;
	};

	/**
	* This class will generate sequenced binary parser following a JSON configuration.
	*/
	class JsonConfigurator: public BinaryParserConfigurator {
	public:
		/**
		* @brief   Construct this class with a valid JSON string. If the string is not valid,
		*          the member function isValid() will return false then.
		*/
		JsonConfigurator(const std::string &json_string);

		/**
		* @brief   Generate sequenced binary parser. This function construct
		*          sequenced binary parser following JSON configuration.
		* @returns
		*          SequencedParser.
		**/
		SequencedParser generateParser() override;

		/**
		* @brief   Indicating that this configurator is valid or not.
		* @returns
		*          bool, true if it is valid, or false if not.
		*/
		bool isValid() override;
	private:
		/**
		* @brief   Generate binary parser with a type parameter which indicates array type or custom type.
		* @param   const char *[in], name from other configuration part.
		*          const char *[in], type description string.
		*          const char *[in], concrete type, used only in custom type condition
		* @returns
		*          BinaryParser instance.
		**/
		BinaryParser *generateParser(const char *name, const char *type, const char*concreteType);

		/**
		* @brief   Compose parser with custom C-struct type. This is done with type information from other
		*          JSON object, so key is used to locate the associated JSON object.
		* @param   const char *[in], a string with name as other JSON object's key
		* @returns
		*          BinaryParser *.
		*/
		BinaryParser *composeParser(const char *key);

		/**
		* @brief   Compose parser with custom C-struct type. This is done with type information from other
		*          JSON object, so key is used to locate the associated JSON object.
		* @param   const char *[in], a string with name as other JSON object's key
		* @returns
		*          BinaryParser *.
		*/
		BinaryParser *composeStructParser(const char *key);

		/**
		* @brief   Compose array parser. This is done with type information from other
		*          JSON object, so key is used to locate the associated JSON object.
		* @param   const char *[in], a string with name as other JSON object's key
		* @returns
		*          BinaryParser *.
		*/
		BinaryParser *composeArrayParser(const char *key);

		ArduinoJson::DynamicJsonDocument _doc;
		ParserFactory _factory;
		bool _valid;
	};
}