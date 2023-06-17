#include "..\Application.h"
#include "..\..\..\PersistanceLayer\Config\Initialization\External Libraries\INIReader\IniReader.h"

#include <iostream>
#include <Windows.h>

Application::Application()
{
	IniData Data = GetIniConfig(GetIniPath());
	
}

bool Application::Run()
{
	IniData Data = GetIniConfig(GetIniPath());

	//OnApplicationStart.Broadcast();

	return true;
}

IniData Application::GetIniConfig(const std::filesystem::path& File)
{
	// To Do : Handle the case where the readers fail in its mission.
	IniReader Reader(File);
	std::cout << "Problematic Lines" << std::endl;
	for (int i : Reader.GetProblematicLines())
		std::cout << std::format("\tProblematic line : {}", i);

	std::cout << std::endl;
	return Reader.GetData();
}

std::filesystem::path Application::GetIniPath()
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