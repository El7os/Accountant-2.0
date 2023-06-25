#include <iostream>
//#include "..\..\PersistanceLayer\Config\Initialization\IniData.h"
#include "..\..\PersistanceLayer\Config\Initialization\IniSection.h"
#include "..\..\PersistanceLayer\Config\Initialization\External Libraries\INIReader\IniReader.h"
#include "..\..\PersistanceLayer\Database\DatabaseController.h"

#include <unordered_set>
#include <any>
#include <filesystem>
#include <Windows.h>
#include "Application.h"

std::filesystem::path GetExePath()
{
	std::filesystem::path Directory;
#ifdef _WIN32
	wchar_t Buffer[2* MAX_PATH];
	GetModuleFileName(NULL, Buffer, sizeof(Buffer));
	Directory = std::filesystem::path(Buffer);
#endif // _WIN32

	Directory._Remove_filename_and_separator();
	return Directory;
}

using Types = Database::SupportedTypes;


int main()
{
	Database::DatabaseController Base (GetExePath() / "Database.db");
	Base.StartConnection(SQLITE_OPEN_READWRITE);

	Base.TerminateConnection();


	



	return 0;
}