#pragma once

#include <filesystem>

#include "External Libraries\SQLite\sqlite3.h"
#include "..\..\External Libraries\Time\Time.h"
#include <initializer_list>
#include <any>

namespace Database
{
	using string = std::string;
	using std::vector;
	using std::tuple;
	using std::any;
	using std::filesystem::path;

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
			const string& Title,
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

		string Title;
		SupportedTypes Type;
		bool NotNull;
		bool PrimaryKey;
		bool AutoIncrement;
		bool Unique;

	};

	struct TableLine
	{
		TableLine() = default;

		TableLine(const vector<any>& Contents)
			: Contents(Contents)
		{
		}

		inline any& operator[](unsigned long int Index)
		{
			return Contents[Index];
		}

		inline const any& operator[](unsigned long int Index) const
		{
			return Contents[Index];
		}

		vector<any> Contents;
	};

	struct Table
	{
		
		Table(const string& Name, const vector<SupportedTypes>& Signature, const vector<string>& ColumnNames)
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

		vector<string> ColumnNames;

		vector<SupportedTypes> Signature;

		vector<TableLine> Rows;

		string Name;
	};

	class DatabaseController
	{
	public:

		using Types = SupportedTypes;

		DatabaseController(const path& File);

		void StartConnection(int Flag = SQLITE_OPEN_READWRITE);
		void TerminateConnection();

		void CreateTable(const string& Title, const vector<ColumnSpec>& Columns);

		void InsertIntoTable(const Table& TargetTable, const TableLine& LineToInsert);
		void InsertIntoTable(const Table& TargetTable, const vector<TableLine>& LinesToInsert);

		void EditRow(Table& Table, const tuple<string, any, Types>& Indicator, const vector<tuple<string, any, Types>>& NewValues);
		
		inline bool IsThereAConnection() const;

		string ConstructQueryCommand(const string& TableName, const vector<string>& TargetColumns = vector<string>(), const string& Condition = "") const;
		string ConstructInsertCommand(const Table& Table, const TableLine& LineToAdd) const;
		string ConstructUpdateRowCommand(Table& Table, const tuple<string, any, Types>& Indicator, const vector<tuple<string, any, Types>>& NewValues);
		string ConstructTableCreationCommand(const string& TableName, const vector<ColumnSpec>& Columns) const;
		string ConstructMultipleInsertCommand(const Table& Table, const vector<TableLine>& LinesToAdd) const;

		inline string GetFileName() const;

		Table* GetTable(const string& TableName);
		Table* GetTable(const string& TableName, const vector<Types>& TableSignature);
		Table* GetTable(const string& TableName, const vector<Types>& TableSignature, const vector<string>& ColumnNames);

		inline const path& GetPath() const;

	protected:

		TableLine ConstructTableLineWithSignature(sqlite3_stmt* Statement,const vector<SupportedTypes>& Signature);
		TableLine ConstructTableLineWithSignature(sqlite3_stmt* Statement, const vector<SupportedTypes>& Signature, int ColumnCount);

		vector<SupportedTypes> ExtractSignature(sqlite3_stmt* Statement);
		vector<string> ExtractColumnNames(sqlite3_stmt* Statement);

		inline string GetAsSqliteString(const any& Value, Types Type) const;

		sqlite3* DbConnection = nullptr;
		int ConnectionFlag;
		const path File;
	};

	inline std::string GetSQLiteType(SupportedTypes Type);
}





