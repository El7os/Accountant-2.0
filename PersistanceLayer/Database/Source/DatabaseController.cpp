#include "..\DatabaseController.h"

#include "..\External Libraries\SQLite\sqlite3.h"

DatabaseController::DatabaseController(const std::filesystem::path& File)
	: File(File)
{
	sqlite3* DatabaseConnection = nullptr;
	sqlite3_open(File.string().c_str(), &DatabaseConnection);
}