#pragma once
#include "json_core.h"

namespace zson 
{
	class JsonParse
	{
	public:
		static std::shared_ptr<JsonElement> ParseObject(const std::string& json, const std::shared_ptr<JsonElement> parent,
														const std::string& field);
		static std::shared_ptr<JsonElement> ParseArray(std::string& json, const std::shared_ptr<JsonElement> parent,
														const std::string& field);
		static std::shared_ptr<std::string> ReverseObject(const JsonObject& pObject);
		static std::shared_ptr<std::string> ReverseArray(const JsonArray& pArray);
	};
}