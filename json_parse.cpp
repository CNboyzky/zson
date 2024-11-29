#include "pch.h"
#include "json_parse.h"
#include "json_util.h"
#include <algorithm>

using namespace zson;

std::shared_ptr<JsonElement> JsonParse::ParseObject(const std::string& json, const std::shared_ptr<JsonElement> parent,
		const std::string& field)
{
	if (json.empty()) {
		throw JsonException("empty json!");
	} 

	// initialize element whose type is OBJECT
	std::shared_ptr<JsonElement> pObject = std::make_shared<JsonElement>();
	pObject->m_field = field;
	pObject->m_parent = &(*parent);
	pObject->m_type = JsonType::OBJECT;
	pObject->m_value = std::make_shared<JsonObject>();


	bool bFoundBegining = false;
	bool bParseField = true;
	std::string fieldName;
	JsonType fieldType = JsonType::UNKNDOWN;
	std::shared_ptr<void> fieldValue = nullptr;
	size_t nPair = 0;
	bool bBasic = true;
	for (size_t i = 0; i < json.size(); i++)
	{
		if (!bFoundBegining)
		{
			if (json[i] == '{') bFoundBegining = true;
			continue;
		}
		
		if (bParseField) // being parsing field ...
		{
			if (json[i] == '"')	// found the begining of the field
			{
				size_t e = JsonUtils::LocateAtStringEnding(json, i, fieldName);
				//fieldName = json.substr(i + 1, e - i - 1);
				i = e;
			}
			// else if (json[i] == '{' || json[i] == '}' || isspace(json[i])) { continue; }
			else if (json[i] == ':') 
			{
				bParseField = false;	// could begin to parse value
				nPair++;
			}
		}
		else  // being parsing value ...
		{
			if (json[i] == '"')		// string
			{
				fieldType = JsonType::STRING;
				std::string strValue;
				size_t e = JsonUtils::LocateAtStringEnding(json, i, strValue);
				fieldValue = std::make_shared<std::string>(strValue);
				// fieldValue = std::make_shared<std::string>(json.substr(i + 1, e - i - 1));
				size_t e1 = JsonUtils::LocateAtValueEnding(json, e);	// to the key-value ending
				
				nPair++;
				i = e1;
				bBasic = true;
			}
			else if (JsonUtils::IsNumber(json[i]))			// number
			{
				size_t e = JsonUtils::LocateAtValueEnding(json, i);	// to the key-value ending
				std::string strNum = json.substr(i, e - i);
				strNum = JsonUtils::Trim(strNum);		// remove space characters
				
				bool bDecimal = JsonUtils::IsDecimal(strNum);

				fieldType = bDecimal ? JsonType::DECIMAL : JsonType::INTEGER;
				//fieldValue = std::make_shared<double>(std::stod(strNum));
				fieldValue = std::make_shared<std::string>(strNum);

				nPair++;
				i = e;
				bBasic = true;
			}
			else if (json[i] == 'T' || json[i] == 't' || json[i] == 'F' || json[i] == 'f') // boolean
			{
				size_t e = JsonUtils::LocateAtValueEnding(json, i);	// to the key-value ending
				std::string strBool = json.substr(i, e - i);
				strBool = JsonUtils::Trim(strBool);		// remove space characters
				std::transform(strBool.begin(), strBool.end(), strBool.begin(), std::tolower);	// to lower case
				
				fieldType = JsonType::BOOL;
				// fieldValue = strBool.compare("true") == 0 ? std::make_shared<bool>(true) : std::make_shared<bool>(false);
				fieldValue = std::make_shared<std::string>(strBool);

				nPair++;
				i = e;
				bBasic = true;
			}
			else if (json[i] == 'N' || json[i] == 'n')	// null
			{
				size_t e = JsonUtils::LocateAtValueEnding(json, i);	// to the key-value ending
				std::string strNull = json.substr(i, e - i);
				strNull = JsonUtils::Trim(strNull);		// remove space characters
				std::transform(strNull.begin(), strNull.end(), strNull.begin(), std::tolower);	// to lower case

				fieldType = JsonType::NUL;
				fieldValue = std::make_shared<std::string>("null");

				nPair++;
				i = e;
				bBasic = true;
			}
			else if (json[i] == '{')	// object
			{
				size_t e = JsonUtils::LocateAtObjectEnding(json, i);	// to the object ending
				std::string strObject = json.substr(i, e + 1 - i);
				size_t e1 = JsonUtils::LocateAtValueEnding(json, e);	// to the key-value ending

				fieldType = JsonType::OBJECT;
				fieldValue = ParseObject(strObject, pObject, fieldName);

				nPair++;
				i = e1;
				bBasic = false;
			}
			else if (json[i] == '[')	// array
			{
				size_t e = JsonUtils::LocateAtArrayEnding(json, i);	// to the array ending
				std::string strArray = json.substr(i, e + 1 - i);
				size_t e1 = JsonUtils::LocateAtValueEnding(json, e);	// to the key-value ending

				fieldType = JsonType::ARRAY;
				fieldValue = ParseArray(strArray, pObject, fieldName);

				nPair++;
				i = e1;
				bBasic = false;
			}
		}

		if (nPair % 2 == 0 && nPair != 0) // complete a pair of field and value
		{
			if (bBasic)
			{
				std::shared_ptr<JsonElement> pChild = std::make_shared<JsonElement>();
				pChild->m_field = fieldName;
				pChild->m_parent = &(*pObject);
				pChild->m_type = fieldType;
				pChild->m_value = fieldValue;
				((std::shared_ptr<JsonObject>&) pObject->m_value)->Add(pChild);
			}
			else
			{
				((std::shared_ptr<JsonObject>&) pObject->m_value)->Add((std::shared_ptr<JsonElement>&) fieldValue);
			}

			bParseField = true;
			fieldName.clear();
			fieldType = JsonType::UNKNDOWN;
			fieldValue = nullptr;
			nPair = 0;
		}
	}

	return pObject;
}

std::shared_ptr<JsonElement> JsonParse::ParseArray(std::string& json, const std::shared_ptr<JsonElement> parent,
	const std::string& field)
{
	if (json.empty()) {
		throw JsonException("empty json!");
	}

	std::shared_ptr<JsonElement> pArray = std::make_shared<JsonElement>();
	pArray->m_field = field;
	pArray->m_parent = &(*parent);
	pArray->m_type = JsonType::ARRAY;
	pArray->m_value = std::make_shared<JsonArray>();

	bool bFoundBeginning = false;
	char buf[10] = { 0 };
	int nCount = 0;
	for (size_t i = 0; i < json.size(); i++)
	{
		if (!bFoundBeginning)
		{
			if (json[i] == '[') bFoundBeginning = true;
			continue;
		}
		else if (json[i] == '"')	// string
		{			
			// parse a string
			std::string strValue;
			size_t e = JsonUtils::LocateAtStringEnding(json, i, strValue);
			std::shared_ptr<void> eleValue = std::make_shared<std::string>(strValue);
			//std::shared_ptr<void> eleValue = std::make_shared<std::string>(json.substr(i + 1, e - i - 1));
			size_t e1 = JsonUtils::LocateAtValueEnding(json, e);	// to the key-value ending

			i = e1;

			// build up child
			std::shared_ptr<JsonElement> pChild = std::make_shared<JsonElement>();
			ZeroMemory(buf, sizeof(buf));
			snprintf(buf, sizeof(buf), "[%d]", nCount++);
			pChild->m_field = buf;
			pChild->m_parent = &(*pArray);
			pChild->m_type = JsonType::STRING;
			pChild->m_value = eleValue;

			((std::shared_ptr<JsonArray>&) pArray->m_value)->Add(pChild);
		}
		else if (JsonUtils::IsNumber(json[i])) // number
		{
			// parse a number
			size_t e = JsonUtils::LocateAtValueEnding(json, i);	// to the key-value ending
			std::string strNum = json.substr(i, e - i);
			strNum = JsonUtils::Trim(strNum);		// remove space characters

			bool bDecimal = JsonUtils::IsDecimal(strNum);

			//std::shared_ptr<void> eleValue = std::make_shared<double>(std::stod(strNum));
			std::shared_ptr<void> eleValue = std::make_shared<std::string>(strNum);

			i = e;

			// build up child
			std::shared_ptr<JsonElement> pChild = std::make_shared<JsonElement>();
			ZeroMemory(buf, sizeof(buf));
			snprintf(buf, sizeof(buf), "[%d]", nCount++);
			pChild->m_field = buf;
			pChild->m_parent = &(*pArray);
			pChild->m_type = bDecimal ? JsonType::DECIMAL : JsonType::INTEGER;
			pChild->m_value = eleValue;

			((std::shared_ptr<JsonArray>&) pArray->m_value)->Add(pChild);
		}
		else if (json[i] == 'T' || json[i] == 't' || json[i] == 'F' || json[i] == 'f')	// boolean
		{
			// parse a boolean
			size_t e = JsonUtils::LocateAtValueEnding(json, i);	// to the key-value ending
			std::string strBool = json.substr(i, e - i);
			strBool = JsonUtils::Trim(strBool);		// remove space characters
			std::transform(strBool.begin(), strBool.end(), strBool.begin(), std::tolower);	// to lower case

			// std::shared_ptr<void> eleValue = strBool.compare("true") == 0 ? std::make_shared<bool>(true) : std::make_shared<bool>(false);
			std::shared_ptr<void> eleValue = std::make_shared<std::string>(strBool);

			i = e;
			
			// build up child
			std::shared_ptr<JsonElement> pChild = std::make_shared<JsonElement>();
			ZeroMemory(buf, sizeof(buf));
			snprintf(buf, sizeof(buf), "[%d]", nCount++);
			pChild->m_field = buf;
			pChild->m_parent = &(*pArray);
			pChild->m_type = JsonType::BOOL;
			pChild->m_value = eleValue;

			((std::shared_ptr<JsonArray>&) pArray->m_value)->Add(pChild);
		}
		else if (json[i] == 'N' || json[i] == 'n')	// null
		{
			// parse null
			size_t e = JsonUtils::LocateAtValueEnding(json, i);	// to the key-value ending
			std::string strNull = json.substr(i, e - i);
			strNull = JsonUtils::Trim(strNull);		// remove space characters
			std::transform(strNull.begin(), strNull.end(), strNull.begin(), std::tolower);	// to lower case

			// std::shared_ptr<void> eleValue = std::make_shared<bool>(0);
			std::shared_ptr<void> eleValue = std::make_shared<std::string>("null");

			i = e;
			
			// build up child
			std::shared_ptr<JsonElement> pChild = std::make_shared<JsonElement>();
			ZeroMemory(buf, sizeof(buf));
			snprintf(buf, sizeof(buf), "[%d]", nCount++);
			pChild->m_field = buf;
			pChild->m_parent = &(*pArray);
			pChild->m_type = JsonType::NUL;
			pChild->m_value = eleValue;

			((std::shared_ptr<JsonArray>&) pArray->m_value)->Add(pChild);
		}
		else if (json[i] == '{')	// object
		{
			// parse an object
			size_t e = JsonUtils::LocateAtObjectEnding(json, i);	// to the object ending
			std::string strObject = json.substr(i, e + 1 - i);
			size_t e1 = JsonUtils::LocateAtValueEnding(json, e);	// to the key-value ending

			ZeroMemory(buf, sizeof(buf));
			snprintf(buf, sizeof(buf), "[%d]", nCount++);
			std::shared_ptr<JsonElement> eleValue = ParseObject(strObject, pArray, buf);

			i = e1;
			((std::shared_ptr<JsonArray>&) pArray->m_value)->Add(eleValue);
		}
		else if (json[i] == '[')	// array
		{
			// parse an array
			size_t e = JsonUtils::LocateAtArrayEnding(json, i);	// to the array ending
			std::string strArray = json.substr(i, e + 1 - i);
			size_t e1 = JsonUtils::LocateAtValueEnding(json, e);	// to the key-value ending

			ZeroMemory(buf, sizeof(buf));
			snprintf(buf, sizeof(buf), "[%d]", nCount++);
			std::shared_ptr<JsonElement> eleValue = ParseArray(strArray, pArray, buf);

			i = e1;
			((std::shared_ptr<JsonArray>&) pArray->m_value)->Add(eleValue);
		}
	}

	return pArray;
}

std::shared_ptr<std::string> JsonParse::ReverseObject(const JsonObject& jsonObject)
{
	std::string json;
	json += "{";
	for (int i = 0; i < jsonObject.GetLength(); i++)
	{
		char buf[20] = { 0 };
		JsonType type = jsonObject.GetTypeAt(i);
		//json += ("\"" + jsonObject.GetFieldAt(i) + "\":");
		json += ("\"" + JsonUtils::RestoreJsonString(jsonObject.GetFieldAt(i)) + "\":");
		switch (type)
		{
		case zson::JsonType::ARRAY:
			json += *(ReverseArray(jsonObject.GetArrayAt(i)));
			break;
		case zson::JsonType::OBJECT:
			json += *(ReverseObject(jsonObject.GetObjectAt(i)));
			break;
		case zson::JsonType::STRING:
			//json += ("\"" + jsonObject.GetStringAt(i) + "\"");
			json += ("\"" + JsonUtils::RestoreJsonString(jsonObject.GetStringAt(i)) + "\"");
			break;
		case zson::JsonType::BOOL:
			json += jsonObject.GetStringAt(i);
			break;
		case zson::JsonType::INTEGER:
			// snprintf(buf, 20, "%.2f", jsonObject.GetNumberAt(i));
			json += jsonObject.GetStringAt(i);
			break;
		case zson::JsonType::DECIMAL:
			json += jsonObject.GetStringAt(i);
			break;
		case zson::JsonType::NUL:
			json += "null";
			break;
		}

		if (i < jsonObject.GetLength() - 1)
		{
			json += ",";
		}
	}
	json += "}";

	std::shared_ptr<std::string> pstr = std::make_shared<std::string>(json);
	return pstr;
}


std::shared_ptr<std::string> JsonParse::ReverseArray(const JsonArray& jsonArray)
{
	std::string json;
	json += "[";
	for (int i = 0; i < jsonArray.GetLength(); i++)
	{
		char buf[20] = { 0 };
		JsonType type = jsonArray.GetTypeAt(i);
		switch (type)
		{
		case zson::JsonType::ARRAY:
			json += *(ReverseArray(jsonArray.GetArrayAt(i)));
			break;
		case zson::JsonType::OBJECT:
			json += *(ReverseObject(jsonArray.GetObjectAt(i)));
			break;
		case zson::JsonType::STRING:
			//json += ("\"" + jsonArray.GetStringAt(i) + "\"");
			json += ("\"" + JsonUtils::RestoreJsonString(jsonArray.GetStringAt(i)) + "\"");
			break;
		case zson::JsonType::BOOL:
			json += jsonArray.GetStringAt(i);
			break;
		case zson::JsonType::INTEGER:
			json += jsonArray.GetStringAt(i);
			break;
		case zson::JsonType::DECIMAL:
			json += jsonArray.GetStringAt(i);
			break;
		case zson::JsonType::NUL:
			json += "null";
			break;
		}

		if (i < jsonArray.GetLength() - 1)
		{
			json += ",";
		}
	}
	json += "]";

	std::shared_ptr<std::string> pstr = std::make_shared<std::string>(json);
	return pstr;
}