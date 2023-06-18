#include "..\DatabaseController.h"
#include "..\..\..\External Libraries\Debug\Macros.h"
#include <string>



std::string Database::GetSQLiteType(Database::SupportedTypes Type)
{
	switch (Type)
	{
	case Database::SupportedTypes::Integer:
		return "INTEGER";

	case Database::SupportedTypes::Text:
		return "TEXT";

	case Database::SupportedTypes::Real:
		return "REAL";

	case Database::SupportedTypes::Null:
		return "NULL";

	default:
		return "";
	}
}

Database::DatabaseController::DatabaseController(const std::filesystem::path& File)
	: File(File)
	, ConnectionFlag(-1)
{
}

bool Database::DatabaseController::IsThereAConnection() const { return DbConnection; }

std::vector<Database::SupportedTypes> Database::DatabaseController::ExtractSignature(sqlite3_stmt* Statement)
{
	const int ColumnCount = sqlite3_column_count(Statement);

	std::vector<SupportedTypes> Signature;
	Signature.resize(ColumnCount);

	for (int ColumnIndex = 0; ColumnIndex < ColumnCount; ++ColumnIndex)
	{
		switch (sqlite3_column_type(Statement,ColumnIndex))
		{
		case SQLITE_INTEGER:
			Signature[ColumnIndex] = SupportedTypes::Integer;
			break;

		case SQLITE_FLOAT:
			Signature[ColumnIndex] = SupportedTypes::Real;
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

std::vector<std::string> Database::DatabaseController::ExtractColumnNames(sqlite3_stmt* Statement)
{
	const int ColumnCount = sqlite3_column_count(Statement);

	std::vector<std::string> ColumnNames;
	ColumnNames.reserve(ColumnCount);

	for (int i = 0; i < ColumnCount; ++i)
		ColumnNames.push_back(sqlite3_column_name(Statement, i));

	return ColumnNames;
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
		LOG(Error, "Database connection attempty failed\n\tAttempted file : {}\n\tError code : {}\n\tError Message : {}", File.string(), ErrorCode,sqlite3_errmsg(DbConnection));
	
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
		LOG(Error
			, "An error occured while terminating the connection with the database ({})\n\tError Code : {}\n\tError Message : {}"
			, File.filename().string()
			, ErrorCode, sqlite3_errmsg(DbConnection));
}

void Database::DatabaseController::CreateTable(const std::string& Title, const std::vector<ColumnSpec>& Columns)
{
	const std::string& SqlCommand = ConstructTableCreationCommand(Title, Columns);
	char* ErrorMessage;
	const int ErrorCode = sqlite3_exec(DbConnection, SqlCommand.c_str(), nullptr, nullptr, &ErrorMessage);

	if (ErrorMessage)
		LOG(Warning, "An error occured while creating a table in {}\n\tError Code : {}\n\tError Message : {}", File.filename().string(), ErrorCode, ErrorMessage)
	else
		LOG(Display, "Table '{}' is successfully created in {}", Title, File.filename().string());
}

void Database::DatabaseController::InsertIntoTable(const Table& TargetTable, const TableLine& LineToInsert)
{
	sqlite3_stmt* Statement = nullptr;

	const std::string& SqlCommand = ConstructInsertCommand(TargetTable, LineToInsert);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqlCommand.c_str(), SqlCommand.size() * sizeof(char), &Statement, nullptr);
	
	LOG(Display, "{}", SqlCommand);
	if (ErrorCode == SQLITE_OK)
	{
		const int StepResult = sqlite3_step(Statement);
		if (StepResult == SQLITE_DONE)
			LOG(Display, "A new line is added to {} table in {}", TargetTable.Name, File.filename().string())
		else
			LOG(Warning, "An error occured while adding a new line to {} table in {}\n\t      Error Code : {}\n\t      Error Message : {}", TargetTable.Name, File.filename().string(), StepResult, sqlite3_errmsg(DbConnection));
	}
	else
		LOG(Warning, "An error occured while adding a new line to {} table in {}\n\t      Error Code : {}\n\t      Error Message : {}", TargetTable.Name, File.filename().string(), ErrorCode, sqlite3_errmsg(DbConnection));
	sqlite3_finalize(Statement);
}

void Database::DatabaseController::InsertIntoTable(const Table& TargetTable, const std::vector<TableLine>& LinesToInsert)
{
	sqlite3_stmt* Statement = nullptr;

	const std::string& SqlCommand = ConstructMultipleInsertCommand(TargetTable, LinesToInsert);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqlCommand.c_str(), SqlCommand.size() * sizeof(char), &Statement, nullptr);

	LOG(Display, "{}", SqlCommand);
	if (ErrorCode == SQLITE_OK)
	{
		const int StepResult = sqlite3_step(Statement);
		if (StepResult == SQLITE_DONE)
			LOG(Display, "A new line is added to {} table in {}", TargetTable.Name, File.filename().string())
		else
			LOG(Warning, "An error occured while adding a new line to {} table in {}\n\t      Error Code : {}\n\t      Error Message : {}", TargetTable.Name, File.filename().string(), StepResult, sqlite3_errmsg(DbConnection));
	}
	else
		LOG(Warning, "An error occured while adding a new line to {} table in {}\n\t      Error Code : {}\n\t      Error Message : {}", TargetTable.Name, File.filename().string(), ErrorCode, sqlite3_errmsg(DbConnection));
	sqlite3_finalize(Statement);
}

void Database::DatabaseController::EditRow(const std::string& TableName, const std::tuple<int, int, Types>, const std::any& Value)
{
}

void Database::DatabaseController::EditRows(const std::string& TableName, const std::vector<std::tuple<int, int, Types>>& Edits)
{
}

Database::Table* Database::DatabaseController::GetTable(const std::string& TableName)
{
	sqlite3_stmt* Statement = nullptr;

	const std::string& SqliteCommand = ConstructQueryCommand(TableName);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqliteCommand.c_str(), (17 + TableName.size()) * sizeof(char), &Statement, nullptr);

	Table* ConstructionTable = nullptr;

	if (ErrorCode == SQLITE_OK)
	{
		const int ColumnCount = sqlite3_column_count(Statement);
		const std::vector<Types>& Signature = ExtractSignature(Statement);
		ConstructionTable = new Table (TableName, Signature, ExtractColumnNames(Statement));
		
		while (sqlite3_step(Statement) == 100)
			ConstructionTable->Rows.push_back(std::move(ConstructTableLineWithSignature(Statement, Signature, ColumnCount)));
		ConstructionTable->Rows.shrink_to_fit();
		LOG(Display, "{} table is fetched succesfully ({})", TableName, File.filename().string());
	}
	else
		LOG(Warning, "An error occured {} while fetching {} table in {} ", ErrorCode, TableName, File.filename().string());
	sqlite3_finalize(Statement);
	return ConstructionTable;
	
}


Database::Table* Database::DatabaseController::GetTable(const std::string& TableName, const std::vector<Types>& TableSignature)
{
	sqlite3_stmt* Statement = nullptr;

	const std::string SqliteCommand = ConstructQueryCommand(TableName);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqliteCommand.c_str(), (17 + TableName.size()) * sizeof(char), &Statement, nullptr);

	Table* ConstructionTable = nullptr;

	if (ErrorCode == SQLITE_OK)
	{
		const int ColumnCount = sqlite3_column_count(Statement) > TableSignature.size() ? TableSignature.size() : sqlite3_column_count(Statement);
		ConstructionTable = new Table(TableName, TableSignature, ExtractColumnNames(Statement));

		while (sqlite3_step(Statement) == 100)
			ConstructionTable->Rows.push_back(std::move(ConstructTableLineWithSignature(Statement, TableSignature, ColumnCount)));

		LOG(Display, "{} table is fetched succesfully ({})", TableName, File.filename().string());
	}
	else
		LOG(Warning, "An error occured ({}) while fetching {} table in {} ", ErrorCode, TableName, File.filename().string());
	sqlite3_finalize(Statement);

	return ConstructionTable;
}

Database::Table* Database::DatabaseController::GetTable(const std::string& TableName, const std::vector<Types>& TableSignature, const std::vector<std::string>& ColumnNames)
{
	sqlite3_stmt* Statement = nullptr;

	const std::string SqliteCommand = ConstructQueryCommand(TableName);
	const int ErrorCode = sqlite3_prepare_v2(DbConnection, SqliteCommand.c_str(), (17 + TableName.size()) * sizeof(char), &Statement, nullptr);

	Table* ConstructionTable = nullptr;

	if (ErrorCode == SQLITE_OK)
	{
		const int ColumnCount = sqlite3_column_count(Statement) > TableSignature.size() ? TableSignature.size() : sqlite3_column_count(Statement);
		ConstructionTable = new Table(TableName, TableSignature, ColumnNames);

		while (sqlite3_step(Statement) == 100)
			ConstructionTable->Rows.push_back(std::move(ConstructTableLineWithSignature(Statement, TableSignature, ColumnCount)));

		LOG(Display, "{} table is fetched succesfully ({})", TableName, File.filename().string());
	}
	else
		LOG(Warning, "An error occured ({}) while fetching {} table in {} ", ErrorCode, TableName, File.filename().string());
	sqlite3_finalize(Statement);

	return ConstructionTable;
}

Database::TableLine Database::DatabaseController::ConstructTableLineWithSignature(sqlite3_stmt* Statement, const std::vector<SupportedTypes>& Signature)
{
	const int ColumnCount = sqlite3_column_count(Statement) > Signature.size() ? Signature.size() : sqlite3_column_count(Statement);
	return ConstructTableLineWithSignature(Statement, Signature, ColumnCount);
}

Database::TableLine Database::DatabaseController::ConstructTableLineWithSignature(sqlite3_stmt* Statement, const std::vector<SupportedTypes>& Signature, int ColumnCount)
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

std::string Database::DatabaseController::ConstructTableCreationCommand(const std::string& TableName, const std::vector<ColumnSpec>& Columns) const
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

std::string Database::DatabaseController::ConstructQueryCommand(const std::string& TableName, const std::vector<std::string>& TargetColumns, const std::string& Condition) const
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
	{
		switch (TableToInsert.Signature[ColumnIndex])
		{
		case Types::Integer:
			InsertPhrase += std::to_string(std::any_cast<int>(LineToInsert[ColumnIndex])) + ", ";
			break;
		case Types::Real:
			InsertPhrase += std::to_string(std::any_cast<double>(LineToInsert[ColumnIndex])) + ", ";
			break;
		case Types::Text:
			InsertPhrase += "'" + std::any_cast<std::string>(LineToInsert[ColumnIndex]) + "', ";
			break;

		default:
			break;
		}
	}

	switch (TableToInsert.Signature[SentinelIndex])
	{
	case Types::Integer:
		InsertPhrase += std::to_string(std::any_cast<int>(LineToInsert[SentinelIndex]));
		break;
	case Types::Real:
	{
		std::string String = std::to_string(std::any_cast<double>(LineToInsert[SentinelIndex]));
		String.pop_back();
		String.pop_back();
		String.pop_back();
		String.pop_back();
		String.pop_back();
		InsertPhrase += String;
		break;

	}
	case Types::Text:
		InsertPhrase += "'" + std::any_cast<std::string>(LineToInsert[SentinelIndex]);
		break;

	default:
		break;
	}

	return InsertPhrase + ");";;
}

std::string Database::DatabaseController::ConstructMultipleInsertCommand(const Table& TableToInsert, const std::vector<TableLine>& LinesToAdd) const
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
		{
			switch (TableToInsert.Signature[ColumnIndex])
			{
			case Types::Integer:
				InsertPhrase += std::to_string(std::any_cast<int>(LineToInsert[ColumnIndex])) + ", ";
				break;
			case Types::Real:
				InsertPhrase += std::to_string(std::any_cast<double>(LineToInsert[ColumnIndex])) + ", ";
				break;
			case Types::Text:
				InsertPhrase += "'" + std::any_cast<std::string>(LineToInsert[ColumnIndex]) + "', ";
				break;

			default:
				break;
			}
		}

		switch (TableToInsert.Signature[SentinelIndex])
		{
		case Types::Integer:
			InsertPhrase += std::to_string(std::any_cast<int>(LineToInsert[SentinelIndex]));
			break;
		case Types::Real:
		{
			std::string String = std::to_string(std::any_cast<double>(LineToInsert[SentinelIndex]));
			String.pop_back();
			String.pop_back();
			String.pop_back();
			String.pop_back();
			String.pop_back();
			InsertPhrase += String;
			break;

		}
		case Types::Text:
			InsertPhrase += "'" + std::any_cast<std::string>(LineToInsert[SentinelIndex]);
			break;

		default:
			break;
		}

		InsertPhrase += "),";
	}
	InsertPhrase.pop_back();
	return InsertPhrase + ";";
}
