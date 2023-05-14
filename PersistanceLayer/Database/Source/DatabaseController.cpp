#include "..\DatabaseController.h"
#include "..\..\..\External Libraries\Debug\Macros.h"

DatabaseController::DatabaseController(const std::filesystem::path& File)
	: File(File)
	, ConnectionFlag(-1)
{
	SqliteRowSpec Row1{ "Name",Types::Text,true,true};
	SqliteRowSpec Row2{ "Id",Types::Integer};
	SqliteRowSpec Row3{ "Surname",Types::Numeric,true,true};
	SqliteRowSpec Row4{ "Salary",Types::Real,true,false};
	SqliteRowSpec Row5{ "Exp",Types::Blob,false,true};
	StartConnection();
	std::vector<SqliteRowSpec> Rows{ Row1, Row2, Row3, Row4, Row5 };
	CreateTable("Employers", Rows);
	TerminateConnection();
}

void DatabaseController::StartConnection(int Flag)
{
	if (IsThereAConnection())
	{
		LOG(Warning, "There is an ongoing connection with the database ({})", File.filename().string());
		return;
	}

	const int ErrorCode = sqlite3_open_v2(File.string().c_str(), &DbConnection,Flag,nullptr);
	if (ErrorCode == SQLITE_OK)
	{
		LOG(Display, "Database connection has been established successfully ({}) with flag {}", File.string(),Flag);
		ConnectionFlag = Flag;
	}
	else
	{
		LOG(Error, "Database connection attempty failed\nAttempted file {}\nError code {}", File.string(), ErrorCode);
	}
	
}

void DatabaseController::TerminateConnection()
{
	if (!IsThereAConnection())
	{
		LOG(Warning, "There is no connection to terminate {}","");
		return;
	}
	const int ErrorCode = sqlite3_close(DbConnection);
	if (ErrorCode == SQLITE_OK)
	{
		LOG(Display, "Database connection has been terminated successfully ({})", File.filename().string());
	}
	else
	{
		LOG(Error, "An error occured while terminating the connection with the database ({})\nError Code {}", File.filename().string(), ErrorCode);
	}
}

void DatabaseController::ChangeTargetFile(const std::filesystem::path& NewFile)
{
	if (DbConnection)
	{
		LOG(Error, "Target database file couldn't be changed, still there is a connection with previous file. Terminate the connection first ({})", File.filename().string());
		return;
	}

	DbConnection = nullptr;
	ConnectionFlag = -1;
	File = NewFile;
}

void DatabaseController::CreateTable(const std::string& Title, const std::vector<SqliteRowSpec>& Rows)
{
	std::string SqlCommand = std::format("CREATE TABLE {}(",Title);
	std::string PrimaryKeys = "";
	bool AutomationFlag = false;
	for (const SqliteRowSpec& RowSpec : Rows)
	{
		std::string Row = RowSpec.Title;
		switch (RowSpec.Type)
		{
		case Types::Integer:
			Row += " INTEGER";
			break;

		case Types::Text:
			Row += " TEXT";
			break;

		case Types::Blob:
			Row += " BLOB";
			break;

		case Types::Real:
			Row += " REAL";
			break;

		case Types::Numeric:
			Row += " NUMERIC";
			break;
		default:
			break;
		}
		if (RowSpec.NotNull)
			Row += " NOT NULL";
		if (RowSpec.Unique)
			Row += " UNIQUE";
		Row += ",";
		SqlCommand += Row;
		if (RowSpec.AutoIncrement)
		{
			PrimaryKeys = std::format("PRIMARY KEY({} AUTOINCREMENT));",RowSpec.Title);
			AutomationFlag = true;
		}
		else if (RowSpec.PrimaryKey && !AutomationFlag)
		{
			PrimaryKeys += RowSpec.Title + ",";
		}

	}
	if (PrimaryKeys == "")
	{
		SqlCommand.pop_back();
		SqlCommand += ");";
	}
	else
	{
		if (AutomationFlag)
			SqlCommand += PrimaryKeys;
		else
		{
			PrimaryKeys.pop_back();
			SqlCommand += std::format("PRIMARY KEY({}));", PrimaryKeys);
		}
	}

	char* ErrorMessage;
	const int ErrorCode = sqlite3_exec(DbConnection, SqlCommand.c_str(), nullptr, nullptr, &ErrorMessage);
	if (ErrorMessage)
	{
		LOG(Warning, "An error occured while creating a table in {}\n\t      Error Code : {}\n\t      Error Message : {}", File.filename().string(), ErrorCode, ErrorMessage);
	}
	else
	{
		LOG(Display, "Table '{}' is successfully created in {}", Title, File.filename().string());
	}
}

bool DatabaseController::IsThereAConnection() { return DbConnection; }

