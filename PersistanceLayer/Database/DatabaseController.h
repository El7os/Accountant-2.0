#pragma once

#include <filesystem>
#include "External Libraries\SQLite\sqlite3.h"


struct SqliteRowSpec
{
	enum class SupportedSqliteTypes : uint8_t
	{
		Integer,
		Text,
		Blob,
		Real,
		Numeric
	};

	SqliteRowSpec(
		const std::string& Title,
		SqliteRowSpec::SupportedSqliteTypes Type,
		bool NotNull = false,
		bool PrimaryKey = false,
		bool AutoIncrement = false,
		bool Unique = false)
		: Title(Title)
		, Type(Type)
		, NotNull(NotNull)
		, PrimaryKey(PrimaryKey)
		, AutoIncrement(AutoIncrement)
		, Unique(Unique)
	{
	}

	std::string Title;
	SupportedSqliteTypes Type;
	bool NotNull;
	bool PrimaryKey;
	bool AutoIncrement;
	bool Unique;


};



class DatabaseController 
{
public:

	using Types = SqliteRowSpec::SupportedSqliteTypes;

	DatabaseController(const std::filesystem::path& File);

	void StartConnection(int Flag = SQLITE_OPEN_READWRITE);
	void TerminateConnection();
	void CreateTable(const std::string& Title, const std::vector<SqliteRowSpec>& Rows);
	void ChangeTargetFile(const std::filesystem::path& NewFile);

	inline bool IsThereAConnection();
protected:
	sqlite3* DbConnection = nullptr;
	int ConnectionFlag;
	std::filesystem::path File;
};