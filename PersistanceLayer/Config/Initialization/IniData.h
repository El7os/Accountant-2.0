#pragma once

#include <vector>
#include <unordered_set>
#include "IniSection.h"

namespace std
{
	template<>
	struct hash<IniSection>
	{
		size_t operator()(const IniSection& Key) const
		{
			return hash<string>::_Do_hash(Key.Name);
		}
	};
}

struct IniData
{
public:

	IniData(const std::string& Name)
		: Name(Name)
	{
	}


	std::string Name;

	std::unordered_set<IniSection> Sections;
};