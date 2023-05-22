#pragma once

#include "DatabaseController.h"

#include <vector>



class UserDatabaseController final : public Database::DatabaseController
{
public:
	
	struct TableLine
	{
		std::string Title;
		std::string Content;
		Time CreationTime;
		Time LastEditTime;
	};


	

	
private:


};