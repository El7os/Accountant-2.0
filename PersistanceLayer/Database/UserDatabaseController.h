#pragma once

#include "DatabaseController.h"

#include <vector>


class UserDatabaseController final : public DatabaseController
{
private:

	struct DatabaseLine
	{
		struct
		{
			uint8_t Hour : 5;
			uint8_t Minute : 6;
			uint8_t Day : 5;
			uint8_t Month : 4;
			int Year;
		}LastEditTime;

		std::string Title;
		std::string Content;
	};



public:



	void CreateTable();

};