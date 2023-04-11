#include "..\DatabaseController.h"
#include "..\SQLite\sqlite3.h"



DatabaseController::DatabaseController()
{

}

void DatabaseController::SetPath(const std::filesystem::path& DBPath)
{
	DataBasePath = DBPath;
}

DatabaseController::EConnectionResult DatabaseController::InitializeConnection()
{
	if (DataBasePath.empty())
		return DatabaseController::EConnectionResult::NoPathFound;
	const char* VFS = "Yahya";
	sqlite3_open_v2(DataBasePath.string().c_str(), &ConnectedDataBase,SQLITE_OPEN_READWRITE,VFS);
	std::cout << sqlite3_errmsg(ConnectedDataBase) << std::endl;
	return ConnectedDataBase ? EConnectionResult::SuccessfulConnection : EConnectionResult::NoConnection;
}

void DatabaseController::TerminateConntection()
{
}