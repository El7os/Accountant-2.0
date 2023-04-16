#pragma once
#include <string>
#include <set>
#include <map>
#include <vector>
#include <filesystem>

#include "..\..\IniData.h"


//struct IniData;
//struct IniSection;
//struct IniProperty;
//
//enum EIniDataType;
/*Reads an .ini file into easy-to-access name/value pairs.*/
class IniReader
{
public:

    static constexpr bool AllowMultiLane   = 1;
    static constexpr bool AllowBom         = 1;
    static constexpr bool UseStack         = 1;
    static constexpr bool StopOnFirstError = 0;

    static constexpr unsigned int MaxLine    = 200U;
    static constexpr unsigned int MaxSection = 50U;
    static constexpr unsigned int MaxName    = 50U;

    /*
    * Constructs an IniReader and parse the File immediately
    * @param File - Indicates the .ini file that about to be parsed.
    */
    IniReader(const std::filesystem::path& File);

    ~IniReader();

    std::vector<std::string> GetSectionNames() const;

    std::vector<std::string> GetFieldNames(const std::string& SectionName) const;

    inline IniData& GetData();

    IniSection* GetSection(const std::string& SectionName) const;

    inline const std::vector<int>& GetProblematicLines() const;

private:

    std::string NormalizeLine(const std::string& Line) const;

    bool IsCommentLine(const std::string& Line) const;

    bool IsSectionLine(const std::string& Line) const;

    bool ParseProperty(const std::string& Line, std::pair<std::string, std::string>& OutProperty) const;

    bool StringToBool(const std::string& String) const;

    IniProperty MakeProperty(const std::pair<std::string, std::string>& RawData) const;

    EIniDataType DetermineType(const std::string& Line) const;

    int8_t ParseFile(const std::filesystem::path& File, std::vector<int>& OutProblematicLines);
    
    std::vector<int> ProblematicLines;

    IniData Data;

};