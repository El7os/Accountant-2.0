#pragma once

#include "..\..\PersistanceLayer\Config\Initialization\IniData.h"
#include <filesystem>

class Application final
{
public:

	Application(const std::filesystem::path& File);


private:
	

	IniData GetIniConfig(const std::filesystem::path& File);
	std::filesystem::path GetIniPath();

};