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
	Database::DatabaseController Base = Database::DatabaseController(GetExePath() / "Source.db");
	Base.StartConnection();
	
	Database::ColumnSpec NameColumn = Database::ColumnSpec("Name", Types::Text, true, true);
	Database::ColumnSpec SurnameColumn = Database::ColumnSpec("Surname", Types::Text, true, true);
	Database::ColumnSpec IdColumn = Database::ColumnSpec("Id", Types::Integer, true, true,false,true);
	Database::ColumnSpec ExperienceColumn = Database::ColumnSpec("Experience", Types::Real, true);
	Base.CreateTable("Employees", std::vector<Database::ColumnSpec>({ NameColumn, SurnameColumn, IdColumn, ExperienceColumn }));

	Database::Table* Employees = Base.GetTable("Employees",
		std::vector<Types>({Types::Text, Types::Text, Types::Integer, Types::Real})
		,std::vector<std::string>({"Name", "Surname", "Id", "Experience"}));
	
	std::any Name = std::make_any<std::string>("John");
	std::any Surname = std::make_any<std::string>("Doe");
	std::any Id = std::make_any<int>(16);
	std::any Id_1 = std::make_any<int>(17);
	std::any Id_2 = std::make_any<int>(18);
	std::any Id_3 = std::make_any<int>(19);
	std::any Id_4 = std::make_any<int>(20);
	std::any Experience = std::make_any<double>(1.5);

	Database::TableLine Line = Database::TableLine({ Name, Surname, Id, Experience });
	Database::TableLine Line_1 = Database::TableLine({ Name, Surname, Id_1, Experience });
	Database::TableLine Line_2 = Database::TableLine({ Name, Surname, Id_2, Experience });
	Database::TableLine Line_3 = Database::TableLine({ Name, Surname, Id_3, Experience });
	Database::TableLine Line_4 = Database::TableLine({ Name, Surname, Id_4, Experience });

	Base.InsertIntoTable(*Employees, std::vector<Database::TableLine>({ Line, Line_1, Line_2, Line_3, Line_4}));
	
	Base.TerminateConnection();



	return 0;
}