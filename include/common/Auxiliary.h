//=============================================================================
/**
* @file    Auxiliary.h
* @version v0.1
* @brief   Auxiliary functions.
*/
//=============================================================================
#pragma once

#include <cstdint>
#include <fstream>
#include <locale>
#include <string>
#include <sstream>
#include <vector>

namespace StorageNS {
	/**
	* @brief   Converting uint8_t to std::string.
	* @param   uint8_t [in]- integral val with uint8_t type. 
	* @returns
	*          std::string represented to corresponding uint8_t val.
	* */
	inline std::string to_string(uint8_t val)
	{
		return std::to_string(static_cast<unsigned long long>(val));
	}

	/**
	* @brief   Converting uint16_t to std::string.
	* @param   uint16_t [in]- integral val with uint16_t type. 
	* @returns
	*          std::string represented to corresponding uint16_t val.
	* */
	inline std::string to_string(uint16_t val)
	{
		return std::to_string(static_cast<unsigned long long>(val));
	}

	/**
	* @brief   Converting uint32_t to std::string.
	* @param   uint32_t [in]- integral val with uint32_t type. 
	* @returns
	*          std::string represented to corresponding uint32_t val.
	* */
	inline std::string to_string(uint32_t val)
	{
		return std::to_string(static_cast<unsigned long long>(val));
	}

	/**
	* @brief   Converting uint64_t to std::string.
	* @param   uint64_t [in]- integral val with uint64_t type. 
	* @returns
	*          std::string represented to corresponding uint64_t val.
	* */
	inline std::string to_string(uint64_t val)
	{
		return std::to_string(static_cast<unsigned long long>(val));
	}

	/**
	* @brief   Converting int8_t to std::string.
	* @param   int8_t [in]- integral val with int8_t type. 
	* @returns
	*          std::string represented to corresponding int8_t val.
	* */
	inline std::string to_string(int8_t val)
	{
		return std::to_string(static_cast<long long>(val));
	}

	/**
	* @brief   Converting int16_t to std::string.
	* @param   int16_t [in]- integral val with int16_t type. 
	* @returns
	*          std::string represented to corresponding int16_t val.
	* */
	inline std::string to_string(int16_t val)
	{
		return std::to_string(static_cast<long long>(val));
	}

	/**
	* @brief   Converting int32_t to std::string.
	* @param   int32_t [in]- integral val with int32_t type. 
	* @returns
	*          std::string represented to corresponding int32_t val.
	* */
	inline std::string to_string(int32_t val)
	{
		return std::to_string(static_cast<long long>(val));
	}

	/**
	* @brief   Converting int64_t to std::string.
	* @param   int64_t [in]- integral val with int64_t type. 
	* @returns
	*          std::string represented to corresponding int64_t val.
	* */
	inline std::string to_string(int64_t val)
	{
		return std::to_string(static_cast<long long>(val));
	}

	/**
	* @brief   Converting float to std::string.
	* @param   float [in]- integral val with float type. 
	* @returns
	*          std::string represented to corresponding float val.
	* */
	inline std::string to_string(float val)
	{
		return std::to_string(static_cast<long double>(val));
	}

	/**
	* @brief   Converting double to std::string.
	* @param   double [in]- float-point val with double type. 
	* @returns
	*          std::string represented to corresponding double val.
	* */
	inline std::string to_string(double val)
	{
		return std::to_string(static_cast<long double>(val));
	}

	/**
	* @brief  Converting std::string to std::wstring
	*/
	inline std::wstring to_wstring(const std::string& str,
		const std::locale& loc = std::locale())
	{
		std::vector<wchar_t> buf(str.size());
		std::use_facet<std::ctype<wchar_t>>(loc).widen(str.data(),
			str.data() + str.size(),
			buf.data());
		return std::wstring(buf.data(), buf.size());
	}

	/**
	* @brief  Converting std::wstring to std::string
	*/
	inline std::string to_string(const std::wstring& str,
		const std::locale& loc = std::locale())
	{
		std::vector<char> buf(str.size());
		std::use_facet<std::ctype<wchar_t>>(loc).narrow(str.data(),
			str.data() + str.size(),
			'?',
			buf.data());
		return std::string(buf.data(), buf.size());
	}

	/**
	* @brief   get content of text file.
	* @param   const char * [in]- text file name. 
	* @returns
	*          std::string - coresponding to the content.
	**/
	inline std::string getTextFileContent(const char *text_file)
	{
		std::ifstream file(text_file);

		if (file) {
			std::ostringstream stream;
			stream << file.rdbuf();
			return stream.str();
		}

		return std::string();
	}

	/**
	* @brief   Calculate the file size with given file stream. The file stream
	*          Should be at least a input stream.
	* @param   std::ifstream [in]- input stream. 
	* @returns
	*          int - the size of given file stream.
	* */
	inline int calculateFileSize(std::ifstream &file)
	{
		auto current = file.tellg();
		file.seekg(0, std::ifstream::end);
		auto size = file.tellg();
		file.seekg(current, std::ifstream::beg);
		return static_cast<int>(size - current);
	}
}