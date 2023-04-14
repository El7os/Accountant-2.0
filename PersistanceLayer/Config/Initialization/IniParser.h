#pragma once

#include <vector>
#include "IniData.h"

class IniParser
{
	
	IniParser& Get()
	{
		static IniParser Parser;
		return Parser;
	}

	int Parse(const std::string& FileName, IniData& Data);

private:

	IniParser();


	IniParser(const IniParser&) = delete;
};