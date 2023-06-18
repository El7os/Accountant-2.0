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
		Real,
		Null,
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
		TableLine() = default;

		TableLine(const std::vector<std::any>& Contents)
			: Contents(Contents)
		{
		}

		inline std::any& operator[](unsigned long int Index)
		{
			return Contents[Index];
		}

		inline const std::any& operator[](unsigned long int Index) const
		{
			return Contents[Index];
		}
		std::vector<std::any> Contents;
	};

	struct Table
	{
		
		Table(const std::string& Name, const std::vector<SupportedTypes>& Signature, const std::vector<std::string>& ColumnNames)
			: Name(Name)
			, Signature(Signature)
			, ColumnNames(ColumnNames)
		{
		}

		inline TableLine& operator[](unsigned long int Index)
		{
			return Rows[Index];
		}

		inline const TableLine& operator[](unsigned long int Index) const
		{
			return Rows[Index];
		}

		inline const size_t RowCount() const
		{
			return Rows.size();
		}

		inline const size_t ColumnCount() const
		{
			return Signature.size();
		}

		std::vector<std::string> ColumnNames;

		std::vector<SupportedTypes> Signature;

		std::vector<TableLine> Rows;

		std::string Name;
	};

	class DatabaseController
	{
	public:

		using Types = SupportedTypes;

		DatabaseController(const std::filesystem::path& File);

		void StartConnection(int Flag = SQLITE_OPEN_READWRITE);
		void TerminateConnection();

		void CreateTable(const std::string& Title, const std::vector<ColumnSpec>& Columns);

		inline bool IsThereAConnection() const;

		std::string ConstructTableCreationCommand(const std::string& TableName, const std::vector<ColumnSpec>& Columns) const;
		std::string ConstructQueryCommand(const std::string& TableName, const std::vector<std::string>& TargetColumns = std::vector<std::string>(), const std::string& Condition = "") const;
		std::string ConstructInsertCommand(const Table& Table, const TableLine& LineToAdd) const;
		std::string ConstructMultipleInsertCommand(const Table& Table, const std::vector<TableLine>& LinesToAdd) const;

		Table* GetTable(const std::string& TableName);
		Table* GetTable(const std::string& TableName, const std::vector<Types>& TableSignature);
		Table* GetTable(const std::string& TableName, const std::vector<Types>& TableSignature, const std::vector<std::string>& ColumnNames);

		void InsertIntoTable(const Table& TargetTable, const TableLine& LineToInsert);
		void InsertIntoTable(const Table& TargetTable, const std::vector<TableLine>& LinesToInsert);


		void EditRow(const std::string& TableName, const std::tuple<int, int, Types>, const std::any& Value);
		void EditRows(const std::string& TableName, const std::vector<std::tuple<int, int, Types>>& Edits);

	protected:

		TableLine ConstructTableLineWithSignature(sqlite3_stmt* Statement,const std::vector<SupportedTypes>& Signature);
		TableLine ConstructTableLineWithSignature(sqlite3_stmt* Statement, const std::vector<SupportedTypes>& Signature, int ColumnCount);

		std::vector<SupportedTypes> ExtractSignature(sqlite3_stmt* Statement);
		std::vector<std::string> ExtractColumnNames(sqlite3_stmt* Statement);

		sqlite3* DbConnection = nullptr;
		int ConnectionFlag;
		const std::filesystem::path File;


	};

	std::string GetSQLiteType(SupportedTypes Type);
}





