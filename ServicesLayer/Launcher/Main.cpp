#include <iostream>
#include "..\..\PersistanceLayer\Config\Initialization\IniData.h"
#include "..\..\PersistanceLayer\Config\Initialization\IniSection.h"
#include <unordered_set>
#include <any>
#include "..\..\PersistanceLayer\Config\Initialization\External Libraries\INIReader\IniReader.h"
#include <filesystem>
#include <Windows.h>





int main()
{
	std::filesystem::path Directory;

#ifdef _WIN32
	wchar_t Buffer[MAX_PATH];
	GetModuleFileName(NULL, Buffer, sizeof(Buffer));
	Directory = std::filesystem::path(Buffer);
#endif // _WIN32

	Directory.replace_filename("Test.ini");
	IniReader Reader(Directory.string());
	//const IniData& Data = Reader.GetData();
	for (auto& Section : Reader.GetSectionNames())
	{
		std::cout << Section << std::endl;
		
	}
	/*for (auto& Section : Reader.GetData().Sections)
	{
		std::cout << Section.Name << std::endl;
		for (auto& Field : Section.Properties)
		{
			std::cout << '\t' << Field.Name << std::endl;
		}
	}*/

	return 0;
}