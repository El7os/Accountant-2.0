#include "..\IniReader.h"

#include <iostream>
#include <fstream>
#include <algorithm>
#include <new>

IniReader::IniReader(const std::filesystem::path& File)
	: Data(File.filename().string())
	, FilePath(File)
{
	ParseErrorCode = ParseFile(File,ProblematicLines);
	ProblematicLines.shrink_to_fit();
}

IniReader::~IniReader()
{
	
}

std::string IniReader::NormalizeLine(const std::string& Line)
{
	size_t First = Line.find_first_not_of(" \t\n\0");
	if (std::string::npos == First)
		return Line;
	
	size_t Last = Line.find_last_not_of(" \0\t\n");
	std::string Result = Line.substr(First, (Last - First + 1));
	Result.shrink_to_fit();
	return Result;
}

bool IniReader::IsCommentLine(const std::string& Line)
{
	return Line[0] == ';' || Line[0] == '#';
}

bool IniReader::IsTrivialLine(const std::string& Line)
{
	return (IsCommentLine(Line) || IsEmptyLine(Line));
}

bool IniReader::IsSectionLine(const std::string& Line)
{
	return Line.starts_with('[') && Line.ends_with(']');
}

bool IniReader::IsEmptyLine(const std::string& Line)
{
	return Line.find_first_not_of(" \0\n") == std::string::npos;
}

bool IniReader::StringToBool(const std::string& String)
{
	if (String._Equal("True") || String._Equal("true"))
		return false;
	else
		return true;
}

bool IniReader::IsBoolean(const std::string& String)
{
	return String._Equal("True") || String._Equal("true") || String._Equal("False") || String._Equal("false");
}

bool IniReader::ParseProperty(const std::string& Line, std::pair<std::string, std::string>& OutProperty)
{
	size_t Length = Line.length();
	size_t AssignmentLocation = Line.find('=', 0);
	if (AssignmentLocation == std::string::npos)
		return false;

	OutProperty.first = NormalizeLine(Line.substr(0, AssignmentLocation - 1));
	OutProperty.second = NormalizeLine(Line.substr(AssignmentLocation + 1, Length - 1));

	return true;
}

IniProperty IniReader::MakeProperty(const std::pair<std::string, std::string>& RawData)
{
	return MakeProperty(RawData, DetermineType(RawData.second));
}

IniProperty IniReader::MakeProperty(const std::pair<std::string, std::string>& RawData, EIniDataType Type)
{
	IniProperty Property(RawData.first);
	switch (Type)
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
	case EIniDataType::EIniDataType_String:
		Property.Type = EIniDataType::EIniDataType_String;
		Property.Data = RawData.second;
		break;
	default:
		Property.Type = EIniDataType::EIniDataType_None;
		Property.Data = RawData.second;
	}
	return Property;

}

EIniDataType IniReader::DetermineType(const std::string& Content)
{
	if (Content.empty())
		return EIniDataType::EIniDataType_None;

	const char FirstChar = Content[0];
	
	if (FirstChar == '"')
	{
		if (Content.ends_with('"'))
			return EIniDataType::EIniDataType_String;
		return EIniDataType::EIniDataType_None;
	}

	if (IsBoolean(Content))
		return EIniDataType::EIniDataType_Bool;
		

	size_t Length = Content.length();
	size_t StartIndex = 0;
	if (FirstChar == '-' || FirstChar == '+')
		++StartIndex;
	
	bool FloatFlag = false;
	for (size_t i = StartIndex; i < Length; ++i)
	{
		if (!isdigit(Content[i]))
		{
			if (Content[i] == '.' && !FloatFlag)
				FloatFlag = true;
			else
				return EIniDataType::EIniDataType_None;
		}
	}
	return FloatFlag ? EIniDataType::EIniDataType_Float : EIniDataType::EIniDataType_Int;
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
	std::vector<std::string> FieldNames;

	if (const IniSection* Section = GetSection(SectionName))
	{
		const std::unordered_set<IniProperty>& Properties = Section->Properties;
		FieldNames.reserve(Properties.size());

		for (const IniProperty& Property : Properties)
			FieldNames.push_back(Property.Name);
	}
	
	return FieldNames;
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

IniReader::EParseError IniReader::ParseFile(const std::filesystem::path& File, std::vector<int>& OutProblematicLines)
{
	std::ifstream Stream(File);
	if (!Stream) 
		return EParseError::FileNotFound;

	std::string Line;

	IniSection CurrentSection("None");
	unsigned long int LineIndex = 0;

	while (std::getline(Stream, Line))
	{
		++LineIndex;
		const std::string& NormalizedLine = NormalizeLine(Line);
		
		if (IsTrivialLine(NormalizedLine))
			continue;

		if (IsSectionLine(NormalizedLine))
		{
			if (CurrentSection.Properties.size())
			{
				Data.Sections.insert(std::move(CurrentSection));
			}
			CurrentSection = IniSection(NormalizedLine.substr(1, strlen(NormalizedLine.c_str()) - 2));
			continue;
		}

		std::pair<std::string, std::string> Property;
		if (!ParseProperty(NormalizedLine, Property))
		{
			ProblematicLines.push_back(LineIndex);
			continue;
		}

		EIniDataType Type = DetermineType(Property.second);
		if (Type == EIniDataType::EIniDataType_None)
		{
			ProblematicLines.push_back(LineIndex);
			continue;
		}
		
		CurrentSection.Properties.insert(MakeProperty(Property,Type));
	}

	if (CurrentSection.Properties.size())
		Data.Sections.insert(std::move(CurrentSection));

	Stream.close();
	return EParseError::SuccesfullyParsed;
}








