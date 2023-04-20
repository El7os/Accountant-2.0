#include "..\Application.h"
#include "..\..\..\PersistanceLayer\Config\Initialization\External Libraries\INIReader\IniReader.h"

#include <iostream>
#include <Windows.h>

Application::Application(const std::filesystem::path& File)
{
	IniData Data = GetIniConfig(File);
	std::cout << std::format("The name of the .ini file : {}", Data.Name) << std::endl;
	for (const IniSection& Section : Data.Sections)
	{
		std::cout << std::format("\t Section {} has {} property(s)", Section.Name, Section.Properties.size()) << std::endl;
		for (const IniProperty& Property : Section.Properties)
		{
			std::cout << std::format("\t\t {}", Property.Name) << std::endl;
		}
	}
}

IniData Application::GetIniConfig(const std::filesystem::path& File)
{
	// To Do : Handle the case where the readers fail in its mission.
	IniReader Reader(File);
	return Reader.GetData();
}

std::filesystem::path GetIniPath()
{
	std::filesystem::path Directory;
#ifdef _WIN32
	wchar_t Buffer[MAX_PATH];
	GetModuleFileName(NULL, Buffer, sizeof(Buffer));
	Directory = std::filesystem::path(Buffer);
#endif // _WIN32

	Directory.replace_filename("Test.ini");
	return Directory;
}