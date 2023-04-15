#pragma once
#include <vector>
#include <utility>
#include <string>
#include <stdint.h>

enum class EIniDataType : uint8_t
{

};
struct IniProperty
{
	
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

	std::string Name;

	EIniDataType Type;

	void* Data = nullptr;

};

class IniSection
{
public:

	void EndBulk()
	{
		Properties.shrink_to_fit();
	}


	std::string SectionName;
	std::vector<IniProperty> Properties;
};

