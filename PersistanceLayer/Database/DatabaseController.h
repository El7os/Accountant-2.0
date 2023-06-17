#pragma once

#include <filesystem>

#include "External Libraries\SQLite\sqlite3.h"
#include "..\..\External Libraries\Time\Time.h"
#include <initializer_list>
#include <any>

namespace Database
{
	enum class SupportedTypes : uint8_t
	{
		Integer = 0U,
		Text,
		Blob,
		Real,
		Null,
		Numeric
	};

	struct ColumnSpec
	{

		ColumnSpec(
			const std::string& Title,
			SupportedTypes Type,
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
		SupportedTypes Type;
		bool NotNull;
		bool PrimaryKey;
		bool AutoIncrement;
		bool Unique;

	};

	struct TableLine
	{
		std::vector<std::any> Contents;
	};

	struct Table
	{
		Table()
		{
		}

		Table(const std::vector< SupportedTypes>& Signature)
			: Signature(Signature)
		{
		}

		std::vector<SupportedTypes> Signature;
		std::vector<TableLine> Rows;

	};

	class DatabaseController
	{
	public:

		using Types = SupportedTypes;

		DatabaseController(const std::filesystem::path& File);

		void StartConnection(int Flag = SQLITE_OPEN_READWRITE);
		void TerminateConnection();
		void CreateTable(const std::string& Title, const std::initializer_list<ColumnSpec>& Columns);

		inline bool IsThereAConnection();

		std::string ConstructTableCreationCommand(const std::string& TableName, const std::initializer_list<ColumnSpec>& Columns);
		std::string ConstructQueryCommand(const std::string& TableName, const std::vector<std::string>& TargetColumns = std::vector<std::string>(), const std::string& Condition = "");

		Table GetTable(const std::string& TableName);
		Table GetTable(const std::string& TableName, const std::vector<Types>& TableSignature);
		Table GetTable(const std::string& TableName, const std::initializer_list<Types>& TableSignature);
	protected:

		TableLine ConstructTableLineWithSignature(sqlite3_stmt* Statement,const std::vector<SupportedTypes>& Signature);
		TableLine ConstructTableLineWithSignature(sqlite3_stmt* Statement, const std::vector<SupportedTypes>& Signature, unsigned long int ColumnCount);

		std::vector<SupportedTypes> ExtractSignature(sqlite3_stmt* Statement);


		sqlite3* DbConnection = nullptr;
		int ConnectionFlag;
		const std::filesystem::path File;


	};
}





