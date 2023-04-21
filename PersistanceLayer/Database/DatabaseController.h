#pragma once

#include <filesystem>

class DatabaseController final
{
public:

	DatabaseController(const std::filesystem::path& File);





private:

	const std::filesystem::path File;
};