#pragma once

#define INI_FILE_NAME "Accountant"

class IniParser
{
	
	IniParser& Get()
	{
		static IniParser Parser;
		return Parser;
	}


	//retreive data
	//parse data






private:

	IniParser();


	IniParser(const IniParser&) = delete;
};