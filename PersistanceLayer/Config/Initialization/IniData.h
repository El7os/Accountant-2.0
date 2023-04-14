#pragma once

#include <vector>
#include "SectionData.h"

class IniData
{
public:

	void EndBulk()
	{
		for (IniSectionData& Section : Sections)
			Section.EndBulk();
		Sections.shrink_to_fit();
	}


	std::vector<IniSectionData> Sections;
};