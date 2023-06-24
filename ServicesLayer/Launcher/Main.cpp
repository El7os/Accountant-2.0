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

	Database::ColumnSpec FirstColumn = { "ID",Types::Integer,true,true,false,true };
	Database::ColumnSpec SecondColumn = { "Name", Types::Text };
	Database::ColumnSpec ThirdColumn = {"Productivity", Types::Real};

	Base.CreateTable("Employers", std::vector<Database::ColumnSpec>({ FirstColumn, SecondColumn, ThirdColumn }));
	Database::Table* Table = Base.GetTable("Employers", std::vector<Types>({ Types::Integer, Types::Text, Types::Real }), std::vector<std::string>({ "ID","Name","Productivity" }));
	

	Database::TableLine Line;
	Line.Contents.push_back(std::make_any<int>(1));
	Line.Contents.push_back(std::make_any<std::string>("Ion"));
	Line.Contents.push_back(std::make_any<double>(0.5));

	Database::TableLine Line2;
	Line2.Contents.push_back(std::make_any<int>(2));
	Line2.Contents.push_back(std::make_any<std::string>("Gheorghe"));
	Line2.Contents.push_back(std::make_any<double>(0.7));

	Database::TableLine Line3;
	Line3.Contents.push_back(std::make_any<int>(3));
	Line3.Contents.push_back(std::make_any<std::string>("Vasile"));
	Line3.Contents.push_back(std::make_any<double>(0.9));

	Database::TableLine Line4;
	Line4.Contents.push_back(std::make_any<int>(4));
	Line4.Contents.push_back(std::make_any<std::string>("Mihai"));
	Line4.Contents.push_back(std::make_any<double>(0.8));

	Base.InsertIntoTable(*Table, Line);
	Base.InsertIntoTable(*Table, std::vector({Line2, Line3}));
	Base.InsertIntoTable(*Table, Line4);

	std::tuple<std::string, std::any, Types> WhereClause = { "ID", std::make_any<int>(1), Types::Integer };
	std::tuple<std::string, std::any, Types> NewValue = { "Name", std::make_any<std::string>("Vasile"), Types::Text };
	std::tuple<std::string, std::any, Types> NewValue2 = { "Productivity", std::make_any<double>(13.3f), Types::Real };


	Base.EditRow(*Table, WhereClause, std::vector<std::tuple<std::string, std::any, Types>>({ NewValue, NewValue2 }));
	Base.RemoveRow(*Table, "ID = 2");

	Base.TerminateConnection();


	



	return 0;
}