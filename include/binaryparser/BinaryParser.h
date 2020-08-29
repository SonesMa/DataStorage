//=============================================================================
/**
* @file    BinaryParser.h
* @version v0.1
* @brief   Binary buffer Parser, supporting basic type buffer, array with basic
           type buffer, ordinary C structure buffer (support nested structure
		   with aforementioned types).
*/
//=============================================================================
#pragma once

#include <exception>
#include <map>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include "Auxiliary.h"

namespace StorageNS {
	typedef std::pair<std::string, std::string> MemberInfo;

	class FormatSpecifier {
	private:
		FormatSpecifier()
		{
			// Default format specifier for basic intrinsic types.
			specifiers[Type::INT8_T] = "%hhd";
			specifiers[Type::INT16_T] = "%hd";
			specifiers[Type::INT32_T] = "%d";
			specifiers[Type::INT64_T] = "%lld";
			specifiers[Type::UINT8_T] = "%hhu";
			specifiers[Type::UINT16_T] = "%hu";
			specifiers[Type::UINT32_T] = "%u";
			specifiers[Type::UINT64_T] = "%llu";
			specifiers[Type::FLOAT] = "%f";
			specifiers[Type::DOUBLE] = "%f";

            delimiter = ", ";
		}

		~FormatSpecifier() {}

	public:
		enum class Type{
			INT8_T,
			INT16_T,
			INT32_T,
			INT64_T,
			UINT8_T,
			UINT16_T,
			UINT32_T,
			UINT64_T,
			FLOAT,
			DOUBLE
		};

		// Singleton pattern
		static FormatSpecifier& instance()
		{
			static FormatSpecifier specifier;

			return specifier;
		}

		/**
		* @brief   Providing setting type specifier if custom is needed.
		* @param   Type type[in]- enumerator to basic intrinsic type
		*          const std::string &[in] - type specifier for formatting
		**/
		void set_specifier(Type type, const std::string &specifier)
		{
			specifiers[type] = specifier;
		}

		const char *get_specifier(Type type)
		{
			return specifiers[type].data();
		}

        void set_delimiter(const std::string &delimiter)
		{
			this->delimiter = delimiter;
		}

        const char *get_delimiter()
		{
			return delimiter.data();
		}
	private:
		std::map<Type, std::string> specifiers;
		std::string delimiter;
	};

	// Function template to provide format specifier in BasicParser 
	template <typename T>
	inline const char* format_specifier();
		
	template <>
	inline const char* format_specifier<int8_t>()
	{
		FormatSpecifier &specifier = FormatSpecifier::instance();
		return specifier.get_specifier(FormatSpecifier::Type::INT8_T);
	}

	template <>
	inline const char* format_specifier<int16_t>()
	{
		FormatSpecifier &specifier = FormatSpecifier::instance();
		return specifier.get_specifier(FormatSpecifier::Type::INT16_T);
	}

	template <>
	inline const char* format_specifier<int32_t>()
	{
		FormatSpecifier &specifier = FormatSpecifier::instance();
		return specifier.get_specifier(FormatSpecifier::Type::INT32_T);
	}
	
	template <>
	inline const char* format_specifier<int64_t>()
	{
		FormatSpecifier &specifier = FormatSpecifier::instance();
		return specifier.get_specifier(FormatSpecifier::Type::INT64_T);
	}

	template <>
	inline const char* format_specifier<uint8_t>()
	{
		FormatSpecifier &specifier = FormatSpecifier::instance();
		return specifier.get_specifier(FormatSpecifier::Type::UINT8_T);
	}		

	template <>
	inline const char* format_specifier<uint16_t>()
	{
		FormatSpecifier &specifier = FormatSpecifier::instance();
		return specifier.get_specifier(FormatSpecifier::Type::UINT16_T);
	}

	template <>
	inline const char* format_specifier<uint32_t>()
	{
		FormatSpecifier &specifier = FormatSpecifier::instance();
		return specifier.get_specifier(FormatSpecifier::Type::UINT32_T);
	}

	template <>
	inline const char* format_specifier<uint64_t>()
	{
		FormatSpecifier &specifier = FormatSpecifier::instance();
		return specifier.get_specifier(FormatSpecifier::Type::UINT64_T);
	}

	template <>
	inline const char* format_specifier<float>()
	{
		FormatSpecifier &specifier = FormatSpecifier::instance();
		return specifier.get_specifier(FormatSpecifier::Type::FLOAT);
	}

	template <>
	inline const char* format_specifier<double>()
	{
		FormatSpecifier &specifier = FormatSpecifier::instance();
		return specifier.get_specifier(FormatSpecifier::Type::DOUBLE);
	}

	/**
	* This class is used to represent null buffer exception.
	*/
	class NullBufferException : std::exception {
	public:
		char const* what() const
		{
			return "Buffer is null";
		}
	};

	/**
	* Binary Parser base class, which defines parse() and length() interfaces.
	*/
	class BinaryParser {
	public:
		virtual ~BinaryParser() {};

		/**
		* @brief   Parse binary buffer.
		* @param   const char *[in]- binary buffer.
		* @returns
		*          std::string - expression in string.
		**/
	    virtual std::string parse(const char* buffer) = 0;

		/**
		* @brief   Parse binary buffer and attach a name to parsed information *          in order to expressing in other place.
		* @param   const std::string &[in] - name descriping this buffer
		* @param   const char *[in]- binary buffer.
		* @returns std::vector<MemberInfo> sequences of parsed information with an order in accordance with binary buffer.
		**/
		virtual std::vector<MemberInfo> expr(const std::string& name, const char* buffer) = 0;

         /**
		* @brief   Parse binary buffer and save parsed data into file.
		* @param   FILE *[in] - file pointer
	    *          const char*[buffer] - buffer without type information
		* @returns
		*          -1 if fail, or 0 if success.
		**/
		virtual int fprintf(FILE *fp, const char* buffer) = 0;

		/**
		* @brief   Character length parsed by this Binary Parser.
		* @returns
		*          Character length parsed by this Binary Parser.
		**/
		virtual size_t length() = 0;
	};

	/**
	* Binary buffer Parser supporting basic data type, including int8_t, int16_t,
	* int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double
	*/
	template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	class BasicParser : public BinaryParser {
	public:
		/**
		* @brief   Parse binary buffer.
		* @param   const char *[in]- binary buffer.
		* @returns
		*          std::string - expression in string.
		**/
	    std::string parse(const char* buffer)
		{
			const int kBASIC_TYPE_EXPR_LENGTH = 64;
			char expr[kBASIC_TYPE_EXPR_LENGTH] = "\0";

			std::sprintf(expr, format_specifier<T>(), buffer);

			return std::string(expr);
		}

		/**
		* @brief   Parse binary buffer and attach a name to parsed information
		*          in order to expressing in other place.
		* @param   const std::string &[in] - name descriping this buffer
		* @param   const char *[in]- binary buffer.
		* @returns std::vector<MemberInfo> sequences of parsed information with an order in accordance with binary buffer.
		**/
		std::vector<MemberInfo> expr(const std::string& name, const char* buffer) override
		{
			std::vector<MemberInfo> members;

			if (buffer == nullptr)
				return members;

			const T* pt = reinterpret_cast<const T*>(buffer);

			members.emplace_back(MemberInfo(name, StorageNS::to_string(*pt)));

			return members;
		}

        /**
		* @brief   Parse binary buffer and save parsed data into file.
		* @param   FILE *[in] - file pointer
	    *          const char*[buffer] - buffer without type information
		* @returns
		*          -1 if fail, or 0 if success.
		**/
		int fprintf(FILE *fp, const char* buffer)
		{
			const T* pt = reinterpret_cast<const T*>(buffer);
			return std::fprintf(fp, format_specifier<T>(), *pt);
		}

		/**
		* @brief   Character length parsed by this Binary Parser.
		* @param   void
		* @returns
		*          Character length parsed by this Binary Parser.
		**/
		size_t length() override
		{
			return sizeof(T);
		}
	};

	/**
	* Binary buffer Parser supporting array type. While element type of the array
	* should be same basic data type as BasicParser. DO NOT support any custom
	* C-struct type.
	*/
	template <typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	class ArrayParser : public BinaryParser {
	public:
		ArrayParser(size_t size) : _size(size) {};

		/**
		* @brief   Parse binary buffer and attach a name to parsed information.
		* @param   const char *[in]- binary buffer.
		* @returns
		*          std::string - expression in string.
		**/
		std::string parse(const char* buffer) override
		{
			if (buffer == nullptr)
				throw NullBufferException();

            const int kARRAY_MAX_SIZE = 256;
			const int kBASIC_TYPE_EXPR_LENGTH = 64;
            const int kARRAY_EXPR_LENGTH = kBASIC_TYPE_EXPR_LENGTH * kARRAY_MAX_SIZE;
			char expr[kARRAY_EXPR_LENGTH];

            std::string array_format_specifier;
			for (size_t i = 0, offset = 0; i < _size; ++i) {
				array_format_specifier += format_specifier<T>();
			}

            std::sprintf(expr, array_format_specifier.data(), buffer);

			return std::string(expr);
		}

		/**
		* @brief   Parse binary buffer and attach a name to parsed information
		*          in order to expressing in other place.
		* @param   const std::string &[in] - name descriping this buffer
		* @param   const char *[in]- binary buffer.
		* @returns std::vector<MemberInfo> sequences of parsed information with an order in accordance with binary buffer.
		**/
		std::vector<MemberInfo> expr(const std::string& name, const char* buffer) override
		{
			std::vector<MemberInfo> members;

			if (buffer == nullptr)
				return members;

			for (size_t i = 0, offset = 0; i < _size; ++i) {
				const T* pt = reinterpret_cast<const T*>(buffer + offset);
				members.emplace_back(MemberInfo(name + "[" + to_string(i) + "]", to_string(*pt)));
				offset += sizeof(T);
			}

			return members;
		}

        /**
		* @brief   Parse binary buffer and save parsed data into file.
		* @param   FILE *[in] - file pointer
	    *          const char*[buffer] - buffer without type information
		* @returns
		*          -1 if fail, or 0 if success.
		**/
		int fprintf(FILE *fp, const char* buffer)
		{
			if (buffer == nullptr)
				throw NullBufferException();

			int result = 0;
			int offset = 0;
			FormatSpecifier& specifier = FormatSpecifier::instance();
			const T* pt;

			for (int i = 0; i < _size; ++i)
			{
				pt = reinterpret_cast<const T*>(buffer + offset);
				std::fprintf(fp, format_specifier<T>(), *pt);
				offset += sizeof(T);
				if (i != _size - 1)
				{
					std::fprintf(fp, "%s", specifier.get_delimiter());
				}
			}

			return result;
		}

		/**
		* @brief   Character length parsed by this Binary Parser.
		* @param   void
		* @returns
		*          Character length parsed by this Binary Parser.
		**/
		size_t length() override
		{
			return _size * sizeof(T);
		}
	private:
		size_t _size;
	};

	/**
	* Binary buffer Parser supporting string type, and the string buffer should be 
	* ended with '\0'.
	*/
	class StringParser : public BinaryParser {
	public:
		StringParser(): _length(0) {}

		/**
		* @brief   Parse binary buffer and attach a name to parsed information.
		* @param   const char *[in]- binary buffer.
		*          const std::string &[in] - name descriping this buffer
		* @returns
		*          std::string - expression in string.
		**/
		std::string parse(const char* buffer) override;

		/**
		* @brief   Parse binary buffer and attach a name to parsed information
		*          in order to expressing in other place.
		* @param   const std::string &[in] - name descriping this buffer
		* @param   const char *[in]- binary buffer.
		* @returns std::vector<MemberInfo> sequences of parsed information with an order in accordance with binary buffer.
		**/
		std::vector<MemberInfo> expr(const std::string& name, const char* buffer) override;

        /**
		* @brief   Parse binary buffer and save parsed data into file.
		* @param   FILE *[in] - file pointer
	    *          const char*[buffer] - buffer without type information
		* @returns
		*          -1 if fail, or 0 if success.
		**/
		int fprintf(FILE *fp, const char* buffer)
		{
			return std::fprintf(fp, "%s", buffer);
		}

		/**
		* @brief   Character length parsed by this Binary Parser.
		* @param   void
		* @returns
		*          Character length parsed by this Binary Parser.
		**/
		size_t length() override;
	private:
		size_t _length;
	};

	/**
	* Binary Parser Factory supporting instantiating binary parser, getting binary parser
	* and release memories acquired from heap by binary parser.
	*/
	class ParserFactory {
	public:
		~ParserFactory();

		/**
		* @brief   Check if a given type string indicated a array type
		* @param   const std::string &[in], type description string
		* @return
		*          true if given type indicating a array type, or false if not
		*/
		bool isArrayType(const std::string &type);

		/**
		* @brief   Check if a given type string indicated a custom type (C-struct).
		* @param   const std::string &[in], type description string
		* @return
		*          true if given type indicating a C-struct type, or false if not
		*/
		bool isStructType(const std::string &type);

		/**
		* @brief   Get parser from parser factory with type string
		* @param   const std::string &[in], type associating parser which has been
		*          added to this factory.
		* @returns
		*          BinaryParser*, instance associated with given type
		*/
		BinaryParser* getParser(const std::string &type);

		/**
		* @brief   Add parser into parser factory with a description type string
		* @param   const std::string &[in], description type string.
		*          BinaryParser *[in], binary parser associating with given type
		*/
		void addParser(const std::string &type, BinaryParser* parser);

		/**
		* @brief   Add parser into parser factory with a description array-type string.
		*          This member function will parse the array-type string into two parts,
		*          i.e. the element type and array-size.
		* @param   const std::string &[in], description type string.
		*/
		BinaryParser *getArrayParser(const std::string &type);
	private:
		BinaryParser* generateArrayParser(const std::string &element_type, int size);

		std::map<std::string, BinaryParser*> parsers;
	};

	/**
	* Sequenced Composited Binary buffer parser. This parser is constructed from 
	* aforementioned binary parser.
	*/
	class SequencedParser : public BinaryParser {
	public:
		SequencedParser(): _length(0) {}

		/**
		* @brief   Parse binary buffer and attach a name to parsed information.
		* @param   const char *[in]- binary buffer.
		* @returns
		*          std::string - expression in string.
		**/
		std::string parse(const char* buffer) override;

		/**
		* @brief   Parse binary buffer and attach a name to parsed information
		*          in order to expressing in other place.
		* @param   const std::string &[in] - name descriping this buffer
		* @param   const char *[in]- binary buffer.
		* @returns std::vector<MemberInfo> sequences of parsed information with an order in accordance with binary buffer.
		**/
		std::vector<MemberInfo> expr(const std::string& name, const char* buffer) override;
		std::vector<MemberInfo> expr(const char* buffer);

        /**
		* @brief   Parse binary buffer and save parsed data into file.
		* @param   FILE *[in] - file pointer
	    *          const char*[buffer] - buffer without type information
		* @returns
		*          -1 if fail, or 0 if success.
		**/
		int fprintf(FILE* fp, const char* buffer);

		/**
		* @brief   Character length parsed by this Binary Parser.
		* @param   void
		* @returns
		*          Character length parsed by this Binary Parser.
		**/
		size_t length() override;

		/**
		* @brief   Add Binary Parser to this sequenced binary parser. This Parser will 
		*          parse binary buffer calling parser in cached binary parser one by one
		*          to support custom data type parsing.
		* @param   const std::string &[in]- name descriping parser parameter.
		*          BinaryParser *[name] - binary parser pointer
		**/
		void addParser(const std::string &name, BinaryParser* parser);
	private:
		std::vector<std::pair<std::string, BinaryParser*>> parsers;
		size_t _length;
	};
}
