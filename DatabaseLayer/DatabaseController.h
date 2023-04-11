#pragma once
#include <iostream>
#include <filesystem>
class sqlite3;

class DatabaseController
{
public:

	enum EConnectionResult
	{
		NoPathFound = 0,
		NoFileFound,
		NoConnection,
		SuccessfulConnection,
	};

	DatabaseController(const DatabaseController&) = delete;

	inline static DatabaseController& GetController()
	{
		static DatabaseController Controller;
		return Controller;
	}

	void SetPath(const std::filesystem::path& DBPath);

	EConnectionResult InitializeConnection();

	void TerminateConntection();


private:
	DatabaseController();

	sqlite3* ConnectedDataBase = nullptr;

	std::filesystem::path DataBasePath;

};

