#include "pch.h"
#include "json_core.h"

using namespace zson;

// ==================== JsonElement =======================
JsonElement::JsonElement()
{
	m_parent = nullptr;
}

JsonElement::~JsonElement()
{
	TRACE(_T("JsonElement released! value: %d\n"), m_value.use_count());
}


// ===================== JsonObject ========================
JsonObject::JsonObject() 
{
	m_length = 0;
}

JsonObject::JsonObject(const JsonObject& other)
{
	this->m_elements = other.m_elements;
	this->m_length = other.m_length;
}

JsonObject::~JsonObject()
{
	this->m_elements.clear();	// clear reference count
	TRACE(_T("JsonObject released!\n"));
}

void JsonObject::Assert(size_t nPos) const
{
	if (nPos < 0 || nPos >= m_length)
	{
		char errors[64] = { 0 };
		sprintf_s<64>(errors, "out of range! bound: [0£¬ %d) but index: %d!", m_length, nPos);
		throw JsonException(errors);
	}
}


const std::shared_ptr<JsonElement> JsonObject::GetAt(size_t nPos) const
{
	Assert(nPos);
	return m_elements[nPos];
}

const std::string& JsonObject::GetFieldAt(size_t nPos) const
{
	Assert(nPos);
	return m_elements[nPos]->m_field;
}

JsonType JsonObject::GetTypeAt(size_t nPos) const
{
	Assert(nPos);
	return m_elements[nPos]->m_type;
}

const std::shared_ptr<void>& JsonObject::GetValueAt(size_t nPos) const
{
	Assert(nPos);
	return m_elements[nPos]->m_value;
}

const std::string& JsonObject::GetStringAt(size_t nPos) const
{
	auto pValue = (const std::shared_ptr<std::string>&) GetValueAt(nPos);
	return *((const std::shared_ptr<std::string>&) pValue);
}

bool JsonObject::GetBoolAt(size_t nPos) const
{
	auto val = GetStringAt(nPos);
	return val.compare("true") == 0;
}

int JsonObject::GetIntAt(size_t nPos) const
{
	auto val = GetStringAt(nPos);
	return std::stoi(val);
}

double JsonObject::GetDoubleAt(size_t nPos) const
{
	auto val = GetStringAt(nPos);
	return std::stod(val);
}

const JsonObject& JsonObject::GetObjectAt(size_t nPos) const
{
	auto pVal = GetValueAt(nPos);
	return *((const std::shared_ptr<JsonObject>&) pVal);
}

const JsonArray& JsonObject::GetArrayAt(size_t nPos) const
{
	auto pVal = GetValueAt(nPos);
	return *((const std::shared_ptr<JsonArray>&) pVal);
}

int JsonObject::GetLength() const
{
	return m_length;
}

void JsonObject::Add(std::shared_ptr<JsonElement> element)
{
	m_elements.push_back(element);
	m_length++;
}

// ========================== JsonArray =============================

JsonArray::JsonArray() 
{
	m_length = 0;
}

JsonArray::JsonArray(const JsonArray& other)
{
	this->m_length = other.m_length;
	this->m_elements = other.m_elements;
}

JsonArray::~JsonArray()
{
	this->m_elements.clear();
	TRACE(_T("JsonArray released!\n"));
}

void JsonArray::Assert(size_t nPos) const
{
	if (nPos < 0 || nPos >= m_length)
	{
		char errors[64] = { 0 };
		sprintf_s<64>(errors, "out of range! bound: [0, %d) but index: %d.", m_length, nPos);
		throw JsonException(errors);
	}
}

int JsonArray::GetLength() const
{
	return m_length;
}


JsonType JsonArray::GetTypeAt(size_t nPos) const
{
	Assert(nPos);
	return m_elements[nPos]->m_type;
}

void JsonArray::Add(std::shared_ptr<JsonElement> element)
{
	m_elements.push_back(element);
	m_length++;
}

const std::shared_ptr<JsonElement> JsonArray::GetAt(size_t nPos) const
{
	Assert(nPos);
	return m_elements[nPos];
}

const std::string& JsonArray::GetStringAt(size_t nPos) const
{
	Assert(nPos);
	const std::shared_ptr<JsonElement> ele = GetAt(nPos);
	return *((const std::shared_ptr<std::string>&) ele->m_value);
}

bool JsonArray::GetBoolAt(size_t nPos) const
{
	Assert(nPos);
	const std::shared_ptr<JsonElement> ele = GetAt(nPos);
	return (*((const std::shared_ptr<std::string>&) ele->m_value)).compare("true") == 0;
}

int JsonArray::GetIntAt(size_t nPos) const
{
	Assert(nPos);
	auto ele = GetAt(nPos);
	return std::stoi(*((const std::shared_ptr<std::string>&) ele->m_value));
}

double JsonArray::GetDoubleAt(size_t nPos) const
{
	Assert(nPos);
	auto ele = GetAt(nPos);
	return std::stod(*((const std::shared_ptr<std::string>&) ele->m_value));
}

const JsonObject& JsonArray::GetObjectAt(size_t nPos) const
{
	Assert(nPos);
	auto ele = GetAt(nPos);
	return *((const std::shared_ptr<JsonObject>&) ele->m_value);
}

const JsonArray& JsonArray::GetArrayAt(size_t nPos) const
{
	Assert(nPos);
	auto ele = GetAt(nPos);
	return *((const std::shared_ptr<JsonArray>&) ele->m_value);
}