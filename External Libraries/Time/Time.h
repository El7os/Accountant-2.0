#pragma once

#include <iostream>

struct Time
{
	uint8_t Second : 6;
	uint8_t Minute : 6;
	uint8_t Hour : 5;

	uint8_t Day : 5;
	uint8_t Month : 4;
	unsigned int Year;
};