#pragma once

#include <vector>
#include "IniSection.h"

class IniData
{
public:

	void EndBulk()
	{
		for (IniSection& Section : Sections)
			Section.EndBulk();
		Sections.shrink_to_fit();
	}


	std::vector<IniSection> Sections;
};