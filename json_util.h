#pragma once
#include "json_util.h"
#include <string>

namespace zson 
{
	class JsonUtils
	{
	public:
		static std::string Trim(const std::string& str);
		static size_t LocateAtStringEnding(const std::string& str, size_t start, std::string& outVal);
		static size_t LocateAtValueEnding(const std::string& str, size_t start);
		static size_t LocateAtObjectEnding(const std::string& str, size_t start);
		static size_t LocateAtArrayEnding(const std::string& str, size_t start);
		static bool IsNumber(const char ch);
		static bool IsDecimal(const std::string& str);
		static int IsJson(const std::string& str);
		static std::string RestoreJsonString(const std::string& original);
	};
}

