#pragma once
#include <vector>
#include <string>
#include <memory>
#include <exception>
#include <stdexcept>

namespace zson 
{
	class JsonArray;

	class JsonException : public std::exception
	{
	private:
		const std::string m_pErr;
	public:
		JsonException(const char* err) : m_pErr(err) {}
		const char* what() throw()
		{
			if (m_pErr.empty())
				return "format json failed!";
			return m_pErr.c_str();
		}
	};


	enum class JsonType
	{
		UNKNDOWN = 0,
		ARRAY,
		OBJECT,
		STRING,
		BOOL,
		INTEGER,
		DECIMAL,
		NUL
	};

	class JsonElement
	{
	public:
		JsonElement();
		~JsonElement();
	public:
		std::string m_field;
		JsonElement* m_parent;
		JsonType	m_type;
		std::shared_ptr<void> m_value;
	};

	//using namespace std;
	class JsonObject
	{
	private:
		std::vector<std::shared_ptr<JsonElement>> m_elements;
		size_t m_length;

	public:
		JsonObject();
		JsonObject(const JsonObject& other);
		~JsonObject();

	public:
		const std::shared_ptr<JsonElement> GetAt(size_t nPos) const;
		const std::string& GetFieldAt(size_t nPos) const;
		JsonType GetTypeAt(size_t nPos) const;
		const std::shared_ptr<void>&  GetValueAt(size_t nPos) const;
		const std::string& GetStringAt(size_t nPos) const;
		bool GetBoolAt(size_t nPos) const;
		int GetIntAt(size_t nPos) const;
		double GetDoubleAt(size_t nPos) const;
		const JsonObject& GetObjectAt(size_t nPos) const;
		const JsonArray& GetArrayAt(size_t nPos) const;
		int GetLength() const;
		void Add(std::shared_ptr<JsonElement> element);

	private:
		void Assert(size_t nPos) const;
	};


	class JsonArray
	{
	private:
		int m_length;
		std::vector<std::shared_ptr<JsonElement>> m_elements;

	public:
		JsonArray();
		JsonArray(const JsonArray& other);
		~JsonArray();
	public:
		const std::shared_ptr<JsonElement> GetAt(size_t nPos) const;
		void Add(std::shared_ptr<JsonElement> element);
		int GetLength() const;
		JsonType GetTypeAt(size_t nPos) const;
		const std::string& GetStringAt(size_t nPos) const;
		bool GetBoolAt(size_t nPos) const;
		int GetIntAt(size_t nPos) const;
		double GetDoubleAt(size_t nPos) const;
		const JsonObject& GetObjectAt(size_t nPos) const;
		const JsonArray& GetArrayAt(size_t nPos) const;
	private:
		void Assert(size_t nPos) const;
	};
}