#pragma once

#include <string>
#include <set>
#include <map>
#include <vector>
#include <filesystem>

#include "..\..\IniData.h"



/*Reads an .ini file into easy-to-access name/value pairs.*/
class IniReader
{
public:

    enum class EParseError : signed char
    {
        FileNotFound = 0,
        SuccesfullyParsed,
    };

    /*
    * Constructs an IniReader and parse the File immediately
    * @param File - Indicates the .ini file that about to be parsed.
    */
    IniReader(const std::filesystem::path& File);

    ~IniReader();

    /*
    * Creates a new string by croping the preceding and following
    * '\t', ' ', '\n' and '\0' characters and uses std::string::shrink_to_fit
    * function on the new string in order to
    * reduce the memory cost of the string.
    * @param Line - The line that wanted to be normalized.
    * @return Normalized version of Line
    */
    static std::string NormalizeLine(const std::string& Line);

    /*
    * Checks wheter the Line is a comment line or not.
    * @param Line - The line that wanted to be checked
    * @return Returns true if the line is a comment line (starting with '#' or ';')
    *         false otherwise
    * @warning Use with normalized lines only.
    */
    static bool IsCommentLine(const std::string& Line);

    /*
    * Checks wheter the Line is a trivial line (comment or empty line).
    * @param Line - The line that wanted to be checked.
    * @return Returns true if the line is a trivial line, false otherwise .
    * @warning Use with normalized lines only.
    */
    static bool IsTrivialLine(const std::string& Line);

    /*
    * Checks wheter the line is a section line
    * @param Line - The line that wanted to be checked.
    * @return Returns true if the line is section line, false otherwise.
    * @warning Use with normalized lines only.
    */
    static bool IsSectionLine(const std::string& Line);

    /*
    * Checks wheter the line is empty
    * @param Line - The line that wanted to be checked.
    * @return Returns true if the line is empty, false otherwise.
    * @warning Use with normalized lines only.
    */
    static bool IsEmptyLine(const std::string& Line);

    /*
    * Checks wheter the line is a string representation of a boolean
    * value ("true","True","false","False").
    * @param Line - The line that wanted to be checked.
    * @return Returns true if the line is a string representation of a boolean
    *         value.
    * @warning Use with normalized lines only.
    */
    static bool IsBoolean(const std::string& Line);

    /*
    * Converts an boolean-trusted(a string that is sure to contain a string representation of a boolean vaalue)
    * string into a boolean value.
    * @param Line - The boolean-truested string to be converted to a boolean value.
    * @return Returns true if the string representation of String contains a value that can
    *         can be reinterpreted as true, false otherwise.
    */
    static bool StringToBool(const std::string& String);

    /*
    * Parsed a line into a pair and writes that pair into OutProperty.
    * @param Line - The line that contains a property pair.
    * @param OutProperty - The pair that the constructed pair is written into.
    * @return Returns true if parsing process is completed without and issues,
    *         false otherwise.
    */
    static bool ParseProperty(const std::string& Line, std::pair<std::string, std::string>& OutProperty);

    /*
    * Constructs an IniProperty from a pair. The first property of the RawData
    * becomes the name of the constructed IniData object and the second one becomes
    * the value of the IniProperty. MakeProperty tries to determine the actual type of the data
    * (supported types float, string, integer, boolean) and rereads and writes the data as that type.
    * If it fails in this task it marks the IniProperty type as 
    * EIniDataType_None and writes the data as string.
    * @param RawData - The pair that wanted to be converted into an IniProperty
    * @return Returns the constructed IniProperty.
    */
    static IniProperty MakeProperty(const std::pair<std::string, std::string>& RawData);

    /*
    * Constructs an IniProperty from a pair. The first property of the RawData
    * becomes the name of the constructed IniData object and the second one becomes
    * the value of the IniProperty.
    * @param RawData - The pair that wanted to be converted into an IniProperty
    * @param Type - The of the data 
    * @return Returns the constructed IniProperty.
    */
    static IniProperty MakeProperty(const std::pair<std::string, std::string>& RawData, EIniDataType Type);

    /*
    * Tries to determine the actual type of the content
    * Supperted Types :
    *       - Float
    *       - String
    *       - Integer
    *       - Boolean
    * @paramm Content - The string whose type is to be determied.
    * @return Returns the corresponding type as an value od EInýDataType.
    *         Returns EIniDataType_None if it fails to determine the actual type
    */
    static EIniDataType DetermineType(const std::string& Content);

    /*
    * Gives the section names of the parsed .ini file.
    * @return Returns the section names in .ini file.
    * @warning Returns an empty vector if the file is empty or
    * couldn't be parsed successfuly.
    */
    std::vector<std::string> GetSectionNames() const;

    /*
    * Finds the fields of given section name and returns its names.
    * @param SectionName - Name of the section whose field's name are wanted
    *        to be listed.
    * @return Returns a list of the names of the fields contained in the section.
    * @warning Returns an empty list if there is no section with specified name.
    */
    std::vector<std::string> GetFieldNames(const std::string& SectionName) const;

    /*
    * Returns the specified section as IniDataPointer.
    * @param SectionName - Name of the section that is wanted to be accessed.
    * @return Returns a pointer to specified section if it exists, nullptr otherwise.
    */
    IniSection* GetSection(const std::string& SectionName) const;

    /*
    * Returns the IniData object the .ini file parsed into.
    * @param A reference to that IniData object.
    */
    inline IniData& GetData() {return Data;}

    /*
    * Gives the path of the .ini file which is parsed.
    * @return Return a std::filesystem::path object that indicates the exact location
    *         of the parsed .ini file.
    */
    inline const std::filesystem::path& GetPath() { return FilePath; }

    /*
    * Gives a list of problematic lines which is determined by the parser.
    * @return A vector list of non-repeating line indexes of problematic lines.
    */
    inline const std::vector<int>& GetProblematicLines() const { return ProblematicLines; }
    
    /*
    * Gives the actual response of the parser to the .ini file.
    * @return Returns the response of the parser as a value of EParserError
    */
    inline EParseError GetParseErrorCode() const { return ParseErrorCode; }

private:

    /*
    * Parses the .ini file which is specified as File into Data class member, and writes  
    * the line indexes of encountered problematic lines into OutProblematicLines.
    * @param File - Specifies the actual location of the file (including the file)
    * @param OutProblematicLines - An int vector that the line indexes of encountered problematic
    *        lines are written into.
    * @return Returns the actual response of the parser as a value of EParserError.
    */
    EParseError ParseFile(const std::filesystem::path& File, std::vector<int>& OutProblematicLines);
    
    std::vector<int> ProblematicLines;

    IniData Data;

    EParseError ParseErrorCode;

    const std::filesystem::path FilePath;
};