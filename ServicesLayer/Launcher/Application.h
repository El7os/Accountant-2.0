#pragma once

#include "..\..\PersistanceLayer\Config\Initialization\IniData.h"
#include <filesystem>
#include "..\..\External Libraries\Delegates\Delegate.h"



class Application final
{
	SECURE_MULTICAST_DELEGATE(NoParamMulticast, Application, void);
public:

	Application();

	NoParamMulticast OnApplicationStart;
	NoParamMulticast OnApplicationEnd;


private:
	


	IniData GetIniConfig(const std::filesystem::path& File);
	std::filesystem::path GetIniPath();

};