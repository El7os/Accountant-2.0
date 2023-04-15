#include <iostream>
#include "..\..\PersistanceLayer\Config\Initialization\IniData.h"
#include "..\..\PersistanceLayer\Config\Initialization\External Libraries\INIReader\IniReader.h"
#include <filesystem>
#include <Windows.h>



int GetIniData(IniData& OutIniData)
{
	std::filesystem::path Directory;
#ifdef _WIN32

	wchar_t Buffer[MAX_PATH];
	GetModuleFileName(NULL, Buffer, sizeof(Buffer));
	Directory = std::filesystem::path(Buffer);

#endif // _WIN32
	Directory.replace_filename("Test.ini");
	IniReader Reader(Directory.string());
	for (const std::string& Section : Reader.GetSections())
	{
		std::cout << Section << std::endl;
		for (const std::string& Field : Reader.GetFields(Section))
		{
			std::cout << '\t' << Field << std::endl;
		}
	}

	return Reader.ParseError();
}



int main()
{
	IniData Data;
	std::cout << GetIniData(Data) << std::endl;

	return 0;
}