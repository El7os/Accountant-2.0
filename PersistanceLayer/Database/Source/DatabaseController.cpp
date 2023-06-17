#include "..\DatabaseController.h"
#include "..\..\..\External Libraries\Debug\Macros.h"
#include <string>


Database::DatabaseController::DatabaseController(const std::filesystem::path& File)
	: File(File)
	, ConnectionFlag(-1)
{
}

void Database::DatabaseController::StartConnection(int Flag)
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
		LOG(Error, "Database connection attempty failed\nAttempted file {}\nError code {}", File.string(), ErrorCode);
	
}

void Database::DatabaseController::TerminateConnection()
{
	if (!IsThereAConnection())
	{
		LOG(Warning, "There is no connection to terminate {}","");
		return;
	}
	const int ErrorCode = sqlite3_close(DbConnection);

	if (ErrorCode == SQLITE_OK)
		LOG(Display, "Database connection has been terminated successfully ({})", File.filename().string())
	else
		LOG(Error, "An error occured while terminating the connection with the database ({})\nError Code {}", File.filename().string(), ErrorCode);
}

void Database::DatabaseController::CreateTable(const std::string& Title, const std::initializer_list<ColumnSpec>& Columns)
{
	const std::string& SqlCommand = ConstructTableCreationCommand(Title, Columns);
	char* ErrorMessage;
	const int ErrorCode = sqlite3_exec(DbConnection, SqlCommand.c_str(), nullptr, nullptr, &ErrorMessage);

	if (ErrorMessage)
		LOG(Warning, "An error occured while creating a table in {}\n\t      Error Code : {}\n\t      Error Message : {}", File.filename().string(), ErrorCode, ErrorMessage)
	else
		LOG(Display, "Table '{}' is successfully created in {}", Title, File.filename().string());
}

std::string Database::DatabaseController::ConstructTableCreationCommand(const std::string& TableName, const std::initializer_list<ColumnSpec>& Columns)
{
	std::string SqlCommand = std::format("CREATE TABLE {}(", TableName);
	std::string PrimaryKeys = "";
	bool AutomationFlag = false;
	for (const ColumnSpec& ColumnSpec : Columns)
	{
		std::string Column = ColumnSpec.Title;
		switch (ColumnSpec.Type)
		{
		case Types::Integer:
			Column += " INTEGER";
			break;

		case Types::Text:
			Column += " TEXT";
			break;

		case Types::Blob:
			Column += " BLOB";
			break;

		case Types::Real:
			Column += " REAL";
			break;

		case Types::Numeric:
			Column += " NUMERIC";
			break;
		default:
			break;
		}
		if (ColumnSpec.NotNull)
			Column += " NOT NULL";
		if (ColumnSpec.Unique)
			Column += " UNIQUE";
		Column += ",";
		SqlCommand += Column;
		if (ColumnSpec.AutoIncrement)
		{
			PrimaryKeys = std::format("PRIMARY KEY({} AUTOINCREMENT));", ColumnSpec.Title);
			AutomationFlag = true;
		}
		else if (ColumnSpec.PrimaryKey && !AutomationFlag)
		{
			PrimaryKeys += ColumnSpec.Title + ",";
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
	return SqlCommand;
}

Database::Table Database::DatabaseController::GetTable(const std::string& TableName, const std::vector<Types>& TableSignature)
{
	sqlite3_stmt* Statement = nullptr;

	const std::string SqliteCommand = ConstructQueryCommand(TableName);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqliteCommand.c_str(), (17 + TableName.size()) * sizeof(char), &Statement, nullptr);

	Table ConstructionTable(TableSignature);

	if (ErrorCode == SQLITE_OK)
	{
		const unsigned long int ColumnCount = sqlite3_column_count(Statement) > TableSignature.size() ? TableSignature.size() : sqlite3_column_count(Statement);

		while (sqlite3_step(Statement) == 100)
			ConstructionTable.Rows.push_back(std::move(ConstructTableLineWithSignature(Statement, TableSignature, ColumnCount)));

		LOG(Display, "{} table is fetched succesfully ({})", TableName, File.filename().string());
	}
	else
		LOG(Warning, "An error occured ({}) while fetching {} table in {} ", ErrorCode, TableName, File.filename().string());
	sqlite3_finalize(Statement);
	
	return ConstructionTable;
}

Database::Table Database::DatabaseController::GetTable(const std::string& TableName)
{
	sqlite3_stmt* Statement = nullptr;

	const std::string& SqliteCommand = ConstructQueryCommand(TableName);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqliteCommand.c_str(), (17 + TableName.size()) * sizeof(char), &Statement, nullptr);

	Table ConstructionTable;

	if (ErrorCode == SQLITE_OK)
	{
		const unsigned int ColumnCount = sqlite3_column_count(Statement);
		const std::vector<SupportedTypes>& Signature = ExtractSignature(Statement);
		ConstructionTable.Signature = Signature;
		
		while (sqlite3_step(Statement) == 100)
			ConstructionTable.Rows.push_back(std::move(ConstructTableLineWithSignature(Statement, Signature, ColumnCount)));
		LOG(Display, "{} table is fetched succesfully ({})", TableName, File.filename().string());
	}
	else
		LOG(Warning, "An error occured {} while fetching {} table in {} ", ErrorCode, TableName, File.filename().string());
	sqlite3_finalize(Statement);
	return ConstructionTable;
	
}

Database::Table Database::DatabaseController::GetTable(const std::string& TableName, const std::initializer_list<Types>& TableSignature)
{
	return GetTable(TableName, std::vector<Types>(TableSignature));
}

Database::TableLine Database::DatabaseController::ConstructTableLineWithSignature(sqlite3_stmt* Statement, const std::vector<SupportedTypes>& Signature)
{
	const unsigned long int ColumnCount = sqlite3_column_count(Statement) > Signature.size() ? Signature.size() : sqlite3_column_count(Statement);
	return ConstructTableLineWithSignature(Statement, Signature, ColumnCount);
}

Database::TableLine Database::DatabaseController::ConstructTableLineWithSignature(sqlite3_stmt* Statement, const std::vector<SupportedTypes>& Signature, unsigned long int ColumnCount)
{
	
	TableLine Line;
	Line.Contents.resize(ColumnCount);

	for (unsigned long int ColumnIndex = 0; ColumnIndex < ColumnCount; ++ColumnIndex)
	{
		switch (Signature[ColumnIndex])
		{
		case Types::Integer:
			Line.Contents[ColumnIndex] = sqlite3_column_int(Statement, ColumnIndex);
			break;
		case Types::Real:
			Line.Contents[ColumnIndex] = sqlite3_column_double(Statement, ColumnIndex);
			break;
		case Types::Text:
			Line.Contents[ColumnIndex] = std::string(reinterpret_cast<const char*>(sqlite3_column_text(Statement, ColumnIndex)));
			break;
		case Types::Blob:
			Line.Contents[ColumnIndex] = sqlite3_column_blob(Statement, ColumnIndex);
			break;
		case Types::Numeric:
			Line.Contents[ColumnIndex] = sqlite3_column_text(Statement, ColumnIndex);
			break;
		default:
			break;
		}
	}
	Line.Contents.shrink_to_fit();
	return Line;
}

std::string Database::DatabaseController::ConstructQueryCommand(const std::string& TableName, const std::vector<std::string>& TargetColumns, const std::string& Condition)
{
	std::string SelectPhrase = "SELECT ";
	if (TargetColumns.size())
	{
		const unsigned int TargetColumnCount = TargetColumns.size();
		for (int i = 0; i < TargetColumnCount - 1; ++i)
			SelectPhrase += TargetColumns[i] + ", ";
		SelectPhrase += TargetColumns[TargetColumnCount - 1];
	}
	else
		SelectPhrase += "* ";
	
	std::string ConditionPhrase = "";
	if (!Condition.empty())
		ConditionPhrase += "WHERE " + Condition;

	return std::format("{} FROM {} {}", SelectPhrase, TableName, ConditionPhrase);
}

bool Database::DatabaseController::IsThereAConnection() { return DbConnection; }

std::vector<Database::SupportedTypes> Database::DatabaseController::ExtractSignature(sqlite3_stmt* Statement)
{
	const unsigned long int ColumnCount = sqlite3_column_count(Statement);

	std::vector<SupportedTypes> Signature;
	Signature.resize(ColumnCount);

	for (unsigned long int ColumnIndex = 0; ColumnIndex < ColumnCount; ++ColumnIndex)
	{
		switch (sqlite3_column_type(Statement,ColumnIndex))
		{
		case SQLITE_INTEGER:
			Signature[ColumnIndex] = SupportedTypes::Integer;
			break;

		case SQLITE_FLOAT:
			Signature[ColumnIndex] = SupportedTypes::Real;
			break;

		case SQLITE_BLOB:
			Signature[ColumnIndex] = SupportedTypes::Blob;
			break;

		case SQLITE_TEXT:
			Signature[ColumnIndex] = SupportedTypes::Text;
			break;

		case SQLITE_NULL:
			Signature[ColumnIndex] = SupportedTypes::Null;
			break;
		default:
			break;
		}
	}
	Signature.shrink_to_fit();
	return Signature;
}
