#include "..\DatabaseController.h"
#include "..\..\..\External Libraries\Debug\Macros.h"
#include <string>



Database::DatabaseController::DatabaseController(const path& File)
	: File(File)
	, ConnectionFlag(-1)
{
	LOG(Display, "A database controller created and paired with file : {}", File.string());
}

void Database::DatabaseController::StartConnection(int Flag)
{
	if (IsThereAConnection())
	{
		LOG(Warning, "There is an ongoing connection with the database ({})", GetFileName());
		return;
	}

	const int ErrorCode = sqlite3_open_v2(File.string().c_str(), &DbConnection,Flag,nullptr);
	if (ErrorCode == SQLITE_OK)
	{
		LOG(Display, "Database connection has been established successfully ({}) with flag {}", File.string(),Flag);
		ConnectionFlag = Flag;
	}
	else
		LOG(Error, "Database connection attempt failed\n\tAttempted file : {}\n\tError code : {}\n\tError Message : {}",
			File.string(),
			ErrorCode,
			sqlite3_errmsg(DbConnection));
	
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
		LOG(Display, "Database connection has been terminated successfully ({})", GetFileName())
	else
		LOG(Error,
			"An error occured while terminating the connection with the database ({})\n\tError Code : {}\n\tError Message : {}", 
			GetFileName(),
			ErrorCode, sqlite3_errmsg(DbConnection));
}

void Database::DatabaseController::CreateTable(const string& Title, const vector<ColumnSpec>& Columns)
{
	const std::string& SqlCommand = ConstructTableCreationCommand(Title, Columns);
	char* ErrorMessage;
	const int ErrorCode = sqlite3_exec(DbConnection, SqlCommand.c_str(), nullptr, nullptr, &ErrorMessage);

	if (ErrorMessage)
		LOG(Warning, "An error occured while creating a table in {}\n\tError Code : {}\n\tError Message : {}",
			GetFileName(),
			ErrorCode,
			ErrorMessage)
	else
		LOG(Display, "Table '{}' is successfully created in {}", Title, GetFileName());
}

void Database::DatabaseController::InsertIntoTable(const Table& TargetTable, const TableLine& LineToInsert)
{
	sqlite3_stmt* Statement = nullptr;

	const std::string& SqlCommand = ConstructInsertCommand(TargetTable, LineToInsert);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqlCommand.c_str(), SqlCommand.size() * sizeof(char), &Statement, nullptr);
	
	if (ErrorCode == SQLITE_OK)
	{
		const int StepResult = sqlite3_step(Statement);
		if (StepResult == SQLITE_DONE)
			LOG(Display, "A new line is added to {} table in {}", TargetTable.Name, GetFileName())
		else
			LOG(Warning,
				"An error occured while adding a new line to {} table in {}\n\tError Code : {}\n\tError Message : {}",
				TargetTable.Name,
				GetFileName(),
				StepResult,
				sqlite3_errmsg(DbConnection));
	}
	else
		LOG(Warning,
			"An error occured while adding a new line to {} table in {}\n\tError Code : {}\n\tError Message : {}",
			TargetTable.Name,
			GetFileName(),
			ErrorCode,
			sqlite3_errmsg(DbConnection));
	sqlite3_finalize(Statement);
}

void Database::DatabaseController::InsertIntoTable(const Table& TargetTable, const vector<TableLine>& LinesToInsert)
{
	sqlite3_stmt* Statement = nullptr;

	const std::string& SqlCommand = ConstructMultipleInsertCommand(TargetTable, LinesToInsert);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqlCommand.c_str(), SqlCommand.size() * sizeof(char), &Statement, nullptr);

	LOG(Display, "{}", SqlCommand);
	if (ErrorCode == SQLITE_OK)
	{
		const int StepResult = sqlite3_step(Statement);
		if (StepResult == SQLITE_DONE)
			LOG(Display, "A new line is added to {} table in {}", TargetTable.Name, GetFileName())
		else
			LOG(Warning,
				"An error occured while adding a new line to {} table in {}\n\tError Code : {}\n\tError Message : {}",
				TargetTable.Name,
				GetFileName(),
				StepResult,
				sqlite3_errmsg(DbConnection));
	}
	else
		LOG(Warning,
			"An error occured while adding a new line to {} table in {}\n\tError Code : {}\n\tError Message : {}",
			TargetTable.Name,
			GetFileName(),
			ErrorCode, 
			sqlite3_errmsg(DbConnection));
	sqlite3_finalize(Statement);
}

void Database::DatabaseController::EditRow(Table& Table, const tuple<string, any, Types>& Indicator, const vector<tuple<string, any, Types>>& NewValues)
{
	sqlite3_stmt* Statement = nullptr;

	const string& SqlCommand = ConstructUpdateRowCommand(Table, Indicator, NewValues);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqlCommand.c_str(), SqlCommand.size() * sizeof(char), &Statement, nullptr);

	if (ErrorCode == SQLITE_OK)
	{

	}
	else
		LOG(Error,
			"An error occured while updating a row at table '{}' in '{}'\n\tError Code : {}\n\tError Mesage : {}",
			Table.Name,
			File.filename().string(),
			ErrorCode,
			sqlite3_errmsg(DbConnection));
	sqlite3_finalize(Statement);
}

bool Database::DatabaseController::IsThereAConnection() const { return DbConnection; }

std::string Database::DatabaseController::ConstructQueryCommand(const string& TableName, const vector<string>& TargetColumns, const string& Condition) const
{
	std::string SelectPhrase = "SELECT ";
	if (TargetColumns.size())
	{
		const size_t TargetColumnCount = TargetColumns.size();
		for (size_t i = 0; i < TargetColumnCount - 1; ++i)
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

std::string Database::DatabaseController::ConstructInsertCommand(const Table& TableToInsert, const TableLine& LineToInsert) const
{
	const size_t ColumnCount = TableToInsert.Signature.size();
	const size_t SentinelIndex = ColumnCount - 1;

	if (!ColumnCount)
	{
		LOG(Error, "Column count is zero in {} table", TableToInsert.Name);
		return "";
	}

	std::string InsertPhrase = "INSERT INTO " + TableToInsert.Name;

	InsertPhrase += " (";
	for (size_t ColumnIndex = 0; ColumnIndex < SentinelIndex; ++ColumnIndex)
		InsertPhrase += TableToInsert.ColumnNames[ColumnIndex] + ", ";
	InsertPhrase += TableToInsert.ColumnNames[SentinelIndex] + ")";

	InsertPhrase += " VALUES(";
	

	for (size_t ColumnIndex = 0; ColumnIndex < SentinelIndex; ++ColumnIndex)
		InsertPhrase += GetAsSqliteString(LineToInsert[ColumnIndex], TableToInsert.Signature[ColumnIndex]) + ", ";
	
	InsertPhrase += GetAsSqliteString(LineToInsert[SentinelIndex], TableToInsert.Signature[SentinelIndex]) + ");";
	
	return InsertPhrase;
}

std::string Database::DatabaseController::ConstructUpdateRowCommand(Table& Table, const tuple<string, any, Types>& Indicator, const vector<tuple<string, any, Types>>& NewValues)
{
	NOT_IMPLEMENTED(string());
	string UpdatePhrase = "UPDATE " + Table.Name + " ";



	return string();
}

std::string Database::DatabaseController::ConstructTableCreationCommand(const string& TableName, const vector<ColumnSpec>& Columns) const
{
	std::string SqlCommand = "CREATE TABLE " + TableName + "("; 
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

		case Types::Real:
			Column += " REAL";
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

std::string Database::DatabaseController::ConstructMultipleInsertCommand(const Table& TableToInsert, const vector<TableLine>& LinesToAdd) const
{
	const size_t ColumnCount = TableToInsert.Signature.size();
	const size_t SentinelIndex = ColumnCount - 1;

	if (!ColumnCount)
	{
		LOG(Error, "Column count is zero in {} table", TableToInsert.Name);
		return "";
	}

	std::string InsertPhrase = "INSERT INTO " + TableToInsert.Name;

	InsertPhrase += " (";
	for (size_t ColumnIndex = 0; ColumnIndex < SentinelIndex; ++ColumnIndex)
		InsertPhrase += TableToInsert.ColumnNames[ColumnIndex] + ", ";
	InsertPhrase += TableToInsert.ColumnNames[SentinelIndex] + ")";

	InsertPhrase += " VALUES";

	for (const TableLine& LineToInsert : LinesToAdd)
	{
		InsertPhrase += "(";
		for (size_t ColumnIndex = 0; ColumnIndex < SentinelIndex; ++ColumnIndex)
			InsertPhrase += GetAsSqliteString(LineToInsert[ColumnIndex], TableToInsert.Signature[ColumnIndex]) + ", ";

		InsertPhrase += GetAsSqliteString(LineToInsert[SentinelIndex], TableToInsert.Signature[SentinelIndex]) + "),";
	}
	InsertPhrase.pop_back();
	return InsertPhrase + ";";
}

std::string Database::DatabaseController::GetFileName() const { return File.filename().string(); }

std::string Database::DatabaseController::GetAsSqliteString(const any& Value, Types Type) const
{
	switch (Type)
	{
		case Types::Integer: return std::to_string(std::any_cast<int>(Value));
		case Types::Real: return std::to_string(std::any_cast<double>(Value));
		case Types::Text: return "'" + std::any_cast<std::string>(Value) + "'";
		default: return "";
	}
}

Database::Table* Database::DatabaseController::GetTable(const string& TableName)
{
	sqlite3_stmt* Statement = nullptr;

	const std::string& SqliteCommand = ConstructQueryCommand(TableName);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqliteCommand.c_str(), SqliteCommand.size() * sizeof(char), &Statement, nullptr);

	Table* ConstructionTable = nullptr;

	if (ErrorCode == SQLITE_OK)
	{
		const int ColumnCount = sqlite3_column_count(Statement);
		const std::vector<Types>& Signature = ExtractSignature(Statement);
		ConstructionTable = new Table (TableName, Signature, ExtractColumnNames(Statement));
		
		while (sqlite3_step(Statement) == 100)
			ConstructionTable->Rows.push_back(std::move(ConstructTableLineWithSignature(Statement, Signature, ColumnCount)));
		ConstructionTable->Rows.shrink_to_fit();
		LOG(Display, "{} table is fetched succesfully ({})", TableName, GetFileName());
	}
	else
		LOG(Warning, "An error occured {} while fetching {} table in {} ", ErrorCode, TableName, GetFileName());
	sqlite3_finalize(Statement);
	return ConstructionTable;
	
}

Database::Table* Database::DatabaseController::GetTable(const string& TableName, const vector<Types>& TableSignature)
{
	sqlite3_stmt* Statement = nullptr;

	const std::string SqliteCommand = ConstructQueryCommand(TableName);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqliteCommand.c_str(), SqliteCommand.size() * sizeof(char), &Statement, nullptr);

	Table* ConstructionTable = nullptr;

	if (ErrorCode == SQLITE_OK)
	{
		const int ColumnCount = sqlite3_column_count(Statement) > TableSignature.size() ? TableSignature.size() : sqlite3_column_count(Statement);
		ConstructionTable = new Table(TableName, TableSignature, ExtractColumnNames(Statement));

		while (sqlite3_step(Statement) == 100)
			ConstructionTable->Rows.push_back(std::move(ConstructTableLineWithSignature(Statement, TableSignature, ColumnCount)));

		LOG(Display, "{} table is fetched succesfully ({})", TableName, GetFileName());
	}
	else
		LOG(Warning, "An error occured ({}) while fetching {} table in {} ", ErrorCode, TableName, GetFileName());
	sqlite3_finalize(Statement);

	return ConstructionTable;
}

Database::Table* Database::DatabaseController::GetTable(const string& TableName, const vector<Types>& TableSignature, const vector<string>& ColumnNames)
{
	sqlite3_stmt* Statement = nullptr;

	const std::string SqliteCommand = ConstructQueryCommand(TableName);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqliteCommand.c_str(), SqliteCommand.size() * sizeof(char), &Statement, nullptr);

	Table* ConstructionTable = nullptr;

	if (ErrorCode == SQLITE_OK)
	{
		const int ColumnCount = sqlite3_column_count(Statement) > TableSignature.size() ? TableSignature.size() : sqlite3_column_count(Statement);
		ConstructionTable = new Table(TableName, TableSignature, ColumnNames);

		while (sqlite3_step(Statement) == 100)
			ConstructionTable->Rows.push_back(std::move(ConstructTableLineWithSignature(Statement, TableSignature, ColumnCount)));

		LOG(Display, "{} table is fetched succesfully ({})", TableName, GetFileName());
	}
	else
		LOG(Warning, "An error occured ({}) while fetching {} table in {} ", ErrorCode, TableName, GetFileName());

	sqlite3_finalize(Statement);

	return ConstructionTable;
}

const std::filesystem::path& Database::DatabaseController::GetPath() const { return File; }

Database::TableLine Database::DatabaseController::ConstructTableLineWithSignature(sqlite3_stmt* Statement, const vector<SupportedTypes>& Signature)
{
	const int ColumnCount = sqlite3_column_count(Statement) > Signature.size() ? Signature.size() : sqlite3_column_count(Statement);
	return ConstructTableLineWithSignature(Statement, Signature, ColumnCount);
}

Database::TableLine Database::DatabaseController::ConstructTableLineWithSignature(sqlite3_stmt* Statement, const vector<SupportedTypes>& Signature, int ColumnCount)
{
	TableLine Line;
	Line.Contents.resize(ColumnCount);

	for (int ColumnIndex = 0; ColumnIndex < ColumnCount; ++ColumnIndex)
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
		default:
			break;
		}
	}
	Line.Contents.shrink_to_fit();
	return Line;
}

std::vector<Database::SupportedTypes> Database::DatabaseController::ExtractSignature(sqlite3_stmt* Statement)
{
	const int ColumnCount = sqlite3_column_count(Statement);

	vector<Types> Signature;
	Signature.resize(ColumnCount);

	for (int ColumnIndex = 0; ColumnIndex < ColumnCount; ++ColumnIndex)
	{
		switch (sqlite3_column_type(Statement,ColumnIndex))
		{
		case SQLITE_INTEGER:
			Signature[ColumnIndex] = Types::Integer;
			break;

		case SQLITE_FLOAT:
			Signature[ColumnIndex] = Types::Real;
			break;

		case SQLITE_TEXT:
			Signature[ColumnIndex] = Types::Text;
			break;

		case SQLITE_NULL:
			Signature[ColumnIndex] = Types::Null;
			break;

		default:
			break;
		}
	}
	Signature.shrink_to_fit();
	return Signature;
}

std::vector<std::string> Database::DatabaseController::ExtractColumnNames(sqlite3_stmt* Statement)
{
	const int ColumnCount = sqlite3_column_count(Statement);

	vector<string> ColumnNames;
	ColumnNames.reserve(ColumnCount);

	for (int i = 0; i < ColumnCount; ++i)
		ColumnNames.push_back(sqlite3_column_name(Statement, i));

	return ColumnNames;
}

std::string Database::GetSQLiteType(SupportedTypes Type) 
{
	switch (Type)
	{
		case Database::SupportedTypes::Integer: return "INTEGER";
		case Database::SupportedTypes::Text: return "TEXT";
		case Database::SupportedTypes::Real: return "REAL";
		case Database::SupportedTypes::Null: return "NULL";
		default: return "";
	}
}















