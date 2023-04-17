#include "..\IniReader.h"



#include <iostream>
#include <fstream>
#include <algorithm>
#include <new>

IniReader::IniReader(const std::filesystem::path& File)
	: Data(File.filename().string())
{
	int ParseError = ParseFile(File,ProblematicLines);
	ProblematicLines.shrink_to_fit();
}

IniReader::~IniReader()
{
	
}

int8_t IniReader::ParseFile(const std::filesystem::path& File, std::vector<int>& OutProblematicLines)
{
	std::ifstream Stream(File);
	if (!Stream) return -1;

	std::string Line;

	IniSection CurrentSection("None");
	unsigned long int LineIndex = 0;

	while (std::getline(Stream, Line))
	{
		++LineIndex;
		const std::string& NormalizedLine = NormalizeLine(Line);
		

		if (IsTrivialLine(NormalizedLine))
			continue;
		std::cout << std::format("[LINE][{}] : {}", LineIndex, NormalizedLine) << std::endl;

		if (IsSectionLine(NormalizedLine))
		{
			std::cout << std::format("[NewSectionDetected][{}] : Section Name = {}", LineIndex, NormalizedLine.substr(1, strlen(NormalizedLine.c_str()) - 2)) << std::endl;
			if (CurrentSection.Properties.size())
			{
				std::cout << std::format("[ASectionInsterted][{}] : Section Name = {} | Property Count = {}", LineIndex, CurrentSection.Name, CurrentSection.Properties.size()) << std::endl;
				Data.Sections.insert(std::move(CurrentSection));
			}
			CurrentSection = IniSection(NormalizedLine.substr(1, strlen(NormalizedLine.c_str()) - 2));
			continue;
		}

		std::pair<std::string, std::string> Property;
		if (!ParseProperty(NormalizedLine, Property))
		{
			std::cout << std::format("[AProblematicLineDetected][{}] : {}", LineIndex, NormalizedLine) << std::endl;
			ProblematicLines.push_back(LineIndex);
			continue;
		}
		std::cout << std::format("[NewProperty][{}][{}] : '{}' = '{}' ", LineIndex, CurrentSection.Name, Property.first, Property.second) << std::endl;

		CurrentSection.Properties.insert(MakeProperty(Property));
	}

	if (CurrentSection.Properties.size())
	{
		std::cout << std::format("[ASectionInsterted][{}] : Section Name = {} | Property Count = {}", LineIndex, CurrentSection.Name, CurrentSection.Properties.size()) << std::endl;
		Data.Sections.insert(std::move(CurrentSection));
	}
	return 0;
}

std::vector<std::string> IniReader::GetSectionNames() const
{
	std::vector<std::string> SectionNames;
	SectionNames.reserve(Data.Sections.size());

	for (const IniSection& Section : Data.Sections)
		SectionNames.push_back(Section.Name);
	SectionNames.shrink_to_fit();
	return SectionNames;
}

std::vector<std::string> IniReader::GetFieldNames(const std::string& SectionName) const
{
	const std::unordered_set<IniProperty>& Properties = GetSection(SectionName)->Properties;
	
	std::vector<std::string> FieldNames;
	FieldNames.reserve(Properties.size());

	for (const IniProperty& Property : Properties)
		FieldNames.push_back(Property.Name);
	return FieldNames;
}

IniData& IniReader::GetData()
{
	return Data;
}

inline const std::vector<int>& IniReader::GetProblematicLines() const
{
	return ProblematicLines;
}

IniSection* IniReader::GetSection(const std::string& SectionName) const
{
	for (const IniSection& Section : Data.Sections)
	{
		if (Section.Name == SectionName)
			return &const_cast<IniSection&>(Section);
	}
	return nullptr;
}

bool IniReader::ParseProperty(const std::string& Line, std::pair<std::string, std::string>& OutProperty) const
{
	size_t Length = strlen(Line.c_str());
	size_t AssignmentLocation = Line.find('=', 0);
	if (AssignmentLocation == std::string::npos)
		return false;

	OutProperty.first = NormalizeLine(Line.substr(0, AssignmentLocation - 1));
	OutProperty.second = NormalizeLine(Line.substr(AssignmentLocation + 1, Length - 1));

	return true;
}

std::string IniReader::NormalizeLine(const std::string& Line) const
{
	size_t First = Line.find_first_not_of(" \t\n\0");
	if (std::string::npos == First)
		return Line;
	
	size_t Last = Line.find_last_not_of(" \t\n");
	return Line.substr(First, (Last - First + 1));
}

bool IniReader::IsCommentLine(const std::string& Line) const
{
	return Line[0] == ';' || Line[0] == '#';
}

bool IniReader::IsSectionLine(const std::string& Line) const
{
	return Line[0] == '[' && Line[strlen(Line.c_str()) - 1] == ']';
}

EIniDataType IniReader::DetermineType(const std::string& Line) const
{
	return EIniDataType::EIniDataType_String;
}

IniProperty IniReader::MakeProperty(const std::pair<std::string, std::string>& RawData) const
{
	IniProperty Property(RawData.first);
	switch (DetermineType(RawData.second))
	{
	case EIniDataType::EIniDataType_Bool:

		Property.Type = EIniDataType::EIniDataType_Bool;
		Property.Data = StringToBool(RawData.second);

		break;
	case EIniDataType::EIniDataType_Int:

		Property.Type = EIniDataType::EIniDataType_Int;
		Property.Data = std::stoi(RawData.second);
		break;
	case EIniDataType::EIniDataType_Float:
	{
		Property.Type = EIniDataType::EIniDataType_Float;
		float Data;
		std::istringstream stream(RawData.second);
		stream >> Data;
		Property.Data = Data;
		break;
	}
	default:
		Property.Type = EIniDataType::EIniDataType_String;
		Property.Data = RawData.second;
		break;
	}
	return Property;

}

bool IniReader::StringToBool(const std::string& String) const
{
	if (String == "True")
		return false;
	else
		return true;
}

bool IniReader::IsTrivialLine(const std::string& Line) const
{
	return (IsCommentLine(Line) || Line.find_first_not_of(" \0\n") == std::string::npos);
}