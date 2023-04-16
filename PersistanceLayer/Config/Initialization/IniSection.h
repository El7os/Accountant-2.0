#pragma once
#include <vector>
#include <utility>
#include <string>
#include <stdint.h>
#include <any>

enum class EIniDataType : int8_t
{
	EIniDataType_None = 0,
	EIniDataType_String,
	EIniDataType_Float,
	EIniDataType_Int,
	EIniDataType_Bool
};
struct IniProperty
{
	IniProperty(const std::string& Name, const std::any& Data, EIniDataType Type)
		: Name(Name)
		, Data(Data)
		, Type(Type)
	{
	}

	IniProperty(const std::string& Name, const std::any& Data)
		: Name(Name)
		, Data(Data)
		, Type(EIniDataType::EIniDataType_None)
	{
	}

	IniProperty(const std::string& Name)
		: Name(Name)
		, Data(nullptr)
		, Type(EIniDataType::EIniDataType_None)
	{
	}

	template<typename T>
	T* Get() const 
	{
		return dynamic_cast<T*>(Data);
	}

	template<typename T>
	const T* GetConst() const
	{
		return dynamic_cast<const T*>(Data);
	}

	inline bool operator==(const IniProperty& Other) const
	{
		return Other.Name == Name;
	}
	std::string Name;

	EIniDataType Type;

	std::any Data;

};

namespace std
{
	template<>
	struct hash<IniProperty>
	{
		size_t operator()(const IniProperty& Key) const
		{
			return hash<string>::_Do_hash(Key.Name);
		}
	};
}

struct IniSection
{
public:

	IniSection(const std::string& Name)
		: Name(Name)
	{
	}

	inline bool operator==(const IniSection& Other) const
	{
		return Other.Name == Name;
	}

	std::string Name;
	std::unordered_set<IniProperty> Properties;
};

