#include "pch.h"
#include "json_util.h"
#include <iterator>
#include "json_core.h"

using namespace zson;
std::string JsonUtils::Trim(const std::string& str)
{
	std::string emptyStr;
	if (str.empty()) str;
	
	int start = -1, end = str.size();
	for (size_t i = 0; i < str.size(); i++)
	{
		if (isspace(str[i])) start++;
		else break;
	}
	for (size_t i = str.size() - 1; i > start; i--)
	{
		if (isspace(str[i])) end--;
		else break;
	}
	
	start++;
	end--;
	if (start > end) return emptyStr;

	return str.substr(start, end + 1 - start);
}

size_t JsonUtils::LocateAtStringEnding(const std::string& str, size_t start, std::string& outVal)
{
	int nCount = 1;
	//char lastCh = '"';
	int continuousSlash = 0;	// continuous count of '\'
	bool bFound = false;
	size_t pos = start + 1;
	for (; pos < str.size(); pos++)
	{
		if (str[pos] == '"' && continuousSlash % 2 == 0) nCount--;
		if (nCount == 0)
		{
			bFound = true;
			break;
		}

		if (str[pos] == '\\')
		{
			continuousSlash++;
			continue;
		}

		// unescaped characters
		if (str[pos] == '"' || str[pos] == 'b' || str[pos] == 'f' || str[pos] == 'n' || str[pos] == 'r' || str[pos] == 't')
		{
			for (int i = 0; i < continuousSlash / 2; i++) outVal.push_back('\\');
			if (continuousSlash % 2 != 0)	// need to reverse
			{
				switch (str[pos])
				{
				case '"':
					outVal.push_back('"');
					break;
				case 'b':
					outVal.push_back('\b');
					break;
				case 'f':
					outVal.push_back('\f');
					break;
				case 'n':
					outVal.push_back('\n');
					break;
				case 'r':
					outVal.push_back('\r');
					break;
				case 't':
					outVal.push_back('\t');
					break;
				}
			}
			else
			{
				switch (str[pos])
				{
				case 'b':
					outVal.push_back('b');
					break;
				case 'f':
					outVal.push_back('f');
					break;
				case 'n':
					outVal.push_back('n');
					break;
				case 'r':
					outVal.push_back('r');
					break;
				case 't':
					outVal.push_back('t');
					break;
				}
			}
		}
		else // normal characters
		{
			for (int i = 0; i < continuousSlash / 2; i++) outVal.push_back('\\');
			outVal.push_back(str[pos]);
		}
		continuousSlash = 0;	// reset continuousSlash to 0
	}

	if (bFound) return pos;
	else
	{
		std::string str1 = str.substr(start, pos + 1 - start);
		char errors[128] = { 0 };
		snprintf(errors, 128, "uncompleted string: %s", str1.c_str());
		throw JsonException(errors);
	}
}


size_t JsonUtils::LocateAtValueEnding(const std::string& str, size_t start)
{
	size_t pos = start + 1;
	bool bFound = false;
	for (; pos < str.size(); pos++)
	{
		if (str[pos] == ',' || str[pos] == '}' || str[pos] == ']')
		{
			bFound = true;
			break;
		}
	}

	if (bFound) return pos;
	else 
	{
		char errors[256] = { 0 };
		snprintf(errors, 256, "uncompleted value! cannot find the ending of a pair of key-value! the string is: %s, the begining index is %d", str.c_str(), start);
		throw JsonException(errors);
	}
}

size_t JsonUtils::LocateAtObjectEnding(const std::string& str, size_t start)
{
	size_t pos = start + 1;
	int nBracketCount = 1;
	int nStrCount = 0;
	//char lastCh = 0;
	int continuousSlash = 0;
	for (; pos < str.size(); pos++)
	{
		if (str[pos] == '"' && continuousSlash % 2 == 0)	// is a string bound
		{
			nStrCount++;
		}

		if (nStrCount % 2 > 0)	// if in a string
		{
			if (str[pos] == '\\') continuousSlash++;
			else continuousSlash = 0;

			continue;
		}

		// not in a string
		if (str[pos] == '{') nBracketCount++;
		else if (str[pos] == '}') nBracketCount--;

		if (nBracketCount == 0) break;
	}

	if (nBracketCount == 0) return pos;
	else
	{
		char errors[256] = { 0 };
		snprintf(errors, 256, "uncompleted object! cannot find the ending of the object! the string is: %s, the begining index is %d", str.c_str(), start);
		throw JsonException(errors);
	}
}

size_t JsonUtils::LocateAtArrayEnding(const std::string& str, size_t start)
{
	size_t pos = start + 1;
	int nBracketCount = 1;
	int nStrCount = 0;
	//char lastCh = 0;
	int continuousSlash = 0;
	for (; pos < str.size(); pos++)
	{
		if (str[pos] == '"' && continuousSlash % 2 == 0)	// is a string bound
		{
			nStrCount++;
		}

		if (nStrCount % 2 > 0)
		{
			if (str[pos] == '\\') continuousSlash++;
			else continuousSlash = 0;
			continue;
		}

		// not in a string
		if (str[pos] == '[') nBracketCount++;
		else if (str[pos] == ']') nBracketCount--;

		if (nBracketCount == 0) break;
	}

	if (nBracketCount == 0) return pos;
	else
	{
		char errors[256] = { 0 };
		snprintf(errors, 256, "uncompleted array! cannot find the ending of the array! the string is: %s, the begining index is %d", str.c_str(), start);
		throw JsonException(errors);
	}
}

bool JsonUtils::IsNumber(const char ch)
{
	if (ch == '-' || ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5'
		|| ch == '6' || ch == '7' || ch == '8' || ch == '9')
	{
		return true;
	}
	return false;
}

bool JsonUtils::IsDecimal(const std::string& str)
{
	bool bFound = false;
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == '.')
		{
			bFound = true;
			break;
		}
	}

	return bFound;
}

int JsonUtils::IsJson(const std::string& str)
{
	int nFlag = 0;
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == '{')
		{
			nFlag = 1;
			break;
		}
		else if (str[i] == '[')
		{
			nFlag = 2;
			break;
		}
	}
	return nFlag;
}

std::string JsonUtils::RestoreJsonString(const std::string& original)
{
	std::string retVal;

	for (size_t i = 0; i < original.size(); i++)
	{
		// escaped characters
		if (original[i] == '"' || original[i] == '\b' || original[i] == '\f' || original[i] == '\n' 
			|| original[i] == '\r' || original[i] == '\t' || original[i] == '\\')
		{ 
			retVal.push_back('\\');

			switch (original[i])
			{
			case '\\':
				retVal.push_back('\\');
				break;
			case '"':
				retVal.push_back('"');
				break;
			case '\b':
				retVal.push_back('b');
				break;
			case '\f':
				retVal.push_back('f');
				break;
			case '\n':
				retVal.push_back('n');
				break;
			case '\r':
				retVal.push_back('r');
				break;
			case '\t':
				retVal.push_back('t');
				break;
			}
		}
		else
		{
			retVal.push_back(original[i]);
		}
	}
	return retVal;
}