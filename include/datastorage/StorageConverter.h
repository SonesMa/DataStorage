#pragma once

#include <fstream>
#include <memory>
#include <string>

#include "BinaryParserConfigurator.h"
#include <cstdio>

namespace StorageNS {
	class StorageConverter {
	public:
		StorageConverter():total_item(0), current_item(0) {}
		virtual ~StorageConverter() {};

		/**
		* @brief  Set binary source file path, which may be absolute or relative.
		*/
		void setBinarySource(const std::string &source_file);

		/**
		* @brief  Set target file path, which may be absolute or relative.
		*/
		void setTargetFile(const std::string &target_file);

		/**
		* @brief  Set template file path, which may be absolute or relative.
		*/
		void setTemplate(const std::string &template_file);

		/**
		* @brief  Get total item counts in binary file.
		*/
		size_t totalItem();

		/**
		* @brief  Get the item count to be processed by next calling convertAndStore().
		*/
		size_t currentItem();

		/**
		* @brief  Check if there has remaining item to be processed.
		*/
		bool hasNext();

		/**
		* @brief  Processing files to prepare for converting and storage.
		*/
		virtual int prepare() = 0;

		/**
		* @brief  Convert binary buffer item to specific decoded item and
		*         store this item to target file.
		*/
		virtual int convertAndStore() = 0;

		virtual int storeHeaders() = 0;

	protected:
		std::string source;
		std::string target;
		std::string template_;

		size_t total_item;
		size_t current_item;
		size_t item_length;
	};

	class CsvStorageConverter: public StorageConverter {
	public:
		/**
		* @brief  Processing files to prepare for converting and storage.
		*         Including configuration of target CSV file, JSON decoding
		*         configurator generation and corresponding parser generation,
		*         and total item count calculating, and so on.
		*/
		int prepare() override;

		/**
		* @brief  Convert binary buffer item to text decoded item and
		*         store this item to target CSV file.
		*/
		int convertAndStore() override;

		int storeHeaders() override;
	private:
		std::FILE *csv_file;
		std::unique_ptr<JsonConfigurator> configurator;
		SequencedParser parsers;
		std::ifstream source_stream;
	};
}