#pragma once

#include <filesystem>
#include <initializer_list>
#include <any>
#include <unordered_map>

#include "External Libraries\SQLite\sqlite3.h"
#include "..\..\External Libraries\Delegates\Delegate.h"


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

		SECURE_MULTICAST_DELEGATE(OnLineChanged, TableLine, const TableLine&);
		SECURE_MULTICAST_DELEGATE(OnLineAdded, TableLine, const TableLine&);
		SECURE_MULTICAST_DELEGATE(OnLineRemoved, TableLine, const TableLine&);

		TableLine() = default;

		TableLine(const vector<any>& Contents)
			: Contents(Contents)
		{
		}


		inline any& operator[](size_t Index)
		{
			return Contents[Index];
		}

		inline const any& operator[](size_t Index) const
		{
			return Contents[Index];
		}

		OnLineChanged LineChanged;
		OnLineAdded LineAdded;
		OnLineRemoved LineRemoved;

		vector<any> Contents;
	};

	struct Table
	{
		

		inline TableLine& operator[](size_t Index) { return Rows[Index]; }

		inline const TableLine& operator[](size_t Index) const { return Rows[Index]; }

		inline const size_t RowCount() const { return Rows.size(); }

		inline const size_t ColumnCount() const { return Signature.size(); }

		inline const string& GetName() const { return Name; }

		inline const vector<string>& GetColumnNames() const { return ColumnNames; }

		inline const vector<SupportedTypes>& GetSignature() const { return Signature; }

		inline const vector<TableLine>& GetRows() const { return Rows; }

	private:

		Table(const string& Name, const vector<SupportedTypes>& Signature, const vector<string>& ColumnNames)
			: Name(Name)
			, Signature(Signature)
			, ColumnNames(ColumnNames)
		{
		}

		Table (const string& Name, const vector<ColumnSpec>& Columns)
			: Name(Name)
		{
			for (const ColumnSpec& Column : Columns)
			{
				ColumnNames.push_back(Column.Title);
				Signature.push_back(Column.Type);
			}
			Signature.shrink_to_fit();
			ColumnNames.shrink_to_fit();
		}

		friend class DatabaseController;

		void UpdateRow(const tuple<string, any, SupportedTypes>& Indicator, const vector<tuple<string, any, SupportedTypes>>& NewValues);

		void RemoveRow(const string& Condition);

		void AddLine(const TableLine& Line);

		void AddLines(const vector<TableLine>& Lines);

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

		void EditRow(Table* Table, const tuple<string, any, Types>& Indicator, const vector<tuple<string, any, Types>>& NewValues);
		void RemoveRow(Table* TargetTable, const string& Condition);
		void CreateTable(const string& Title, const vector<ColumnSpec>& Columns);
		void StartConnection(int Flag = SQLITE_OPEN_READWRITE);
		void InsertIntoTable(Table* TargetTable, const TableLine& LineToInsert);
		void InsertIntoTable(Table* TargetTable, const vector<TableLine>& LinesToInsert);
		void TerminateConnection();

		inline bool IsThereAConnection() const;

		string ConstructQueryCommand(const string& TableName, const vector<string>& TargetColumns = vector<string>(), const string& Condition = "") const;
		string ConstructInsertCommand(const Table* Table, const TableLine& LineToAdd) const;
		string ConstructDeleteRowCommand(const string& TableName, const string& Condition) const;
		string ConstructUpdateRowCommand(const Table* Table, const tuple<string, any, Types>& Indicator, const vector<tuple<string, any, Types>>& NewValues) const;
		string ConstructTableCreationCommand(const string& TableName, const vector<ColumnSpec>& Columns) const;
		string ConstructMultipleInsertCommand(const Table* Table, const vector<TableLine>& LinesToAdd) const;

		inline string GetFileName() const;

		Table* GetTable(const string& TableName);
		/*Table* GetTable(const string& TableName, const vector<Types>& TableSignature);
		Table* GetTable(const string& TableName, const vector<Types>& TableSignature, const vector<string>& ColumnNames);*/

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

	private:

		std::unordered_map<string, Table*> Tables;
	};

	
	std::string GetSQLiteType(SupportedTypes Type);
}





