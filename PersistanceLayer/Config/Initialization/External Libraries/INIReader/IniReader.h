#pragma once
#include <string>
#include <set>
#include <map>
#include <filesystem>

/*
Nonzero to allow multi-line value parsing, in the style of Python's
ConfigParser. If allowed, ini_parse() will call the handler with the same
name for each subsequent line parsed.
*/
#ifndef INI_ALLOW_MULTILANE
#define INI_ALLOW_MULTILANE 1
#endif

/*
Nonzero to allow a UTF-8 BOM sequence (0xEF 0xBB 0xBF) at the start of
the file. See http://code.google.com/p/inih/issues/detail?id=21
*/
#ifndef INI_ALLOW_BOM
#define INI_ALLOW_BOM 1
#endif

/* Nonzero to use stack, zero to use heap (malloc/free). */
#ifndef INI_USE_STACK
#define INI_USE_STACK 1
#endif

/* Stop parsing on first error (default is to keep parsing). */
#ifndef INI_STOP_ON_FIRST_ERROR
#define INI_STOP_ON_FIRST_ERROR 0
#endif

/* Maximum line length for any line in INI file. */
#ifndef INI_MAX_LINE
#define INI_MAX_LINE 200
#endif

#if !INI_USE_STACK
#include <stdlib.h>
#endif

#define MAX_SECTION 50
#define MAX_NAME 50

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

    /*
    * Gives the encountered pars error if it is exists. 0 on success, line number of
    * first error on parse error, or -1 on file open error.
    * @return - Parse error
    */
    inline int ParseError() const
    {
        return _error;
    }

    /*
    * Gets a string value from .ini file if it is exists.
    * @param Section - The section where the field is located.
    * @param Name - Name of the field which is looked for.
    * @param OutData - Value of the field is written into this string if it is found
    * @return Returns true if the operation is managed successfuly, false otherwise.
    */
    bool Get(const std::string& Section, const std::string& Name, std::string& OutData);

    /*
    * Gets an integer (long) value from .ini file if it is exists
    * an valid (decimal or hex)
    * @param Section - The section where the field is located.
    * @param Name - Name of the field which is looked for.
    * @param OutData - Value of the field is written into this long if it is found
    * @return Returns true if the operation is managed successfuly, false otherwise.
    */
    bool GetInteger(const std::string& Section, const std::string& Name, long& OutData);

    /*
    * Gets an float value from .ini file if it is exists and valid.
    * @param Section - The section where the field is located.
    * @param Name - Name of the field which is looked for.
    * @param OutData - Value of the field is written into this double if it is found
    * @return Returns true if the operation is managed successfuly, false otherwise.
    */
    bool GetFloat(const std::string& Section, const std::string& Name, double& OutData);

    /*
    * Gets a boolean value from .ini file if it is exists and valid.
    * "True", "Yes", "On" and "1" values are accepted as true and
    * "False", "No", "Off" and "0" values are accepted as false (not case sensitive).
    * @param Section - The section where the field is located.
    * @param Name - Name of the field which is looked for.
    * @param OutData - Value of the field is written into this bool if it is found
    * @return Returns true if the operation is managed successfuly, false otherwise.
    */
    bool GetBoolean(const std::string& Section, const std::string& Name, bool& OutData);

    /*
    * Returns all the section names in the .ini file, in alphabetical order, but in the orginal casing.
    * @return A set that contains all sections in the .ini file.
    */
    inline const std::set<std::string>& GetSections() const
    {
        return _sections;
    }

    /*
    * Returns all the filed names in the .ini file, in alphhabetical order but in the orginal
    * casing.
    * @param Section - The name of the section which is the names of its fields wanted.
    * @return Returns the names of the fields in the specified Section. Returns an empty set
    *         if the field name is unknown.
    */
    std::set<std::string> GetFields(const std::string& Section) const;

private:

    static std::string MakeKey(const std::string& Section, const std::string& Name);

    static int ValueHandler(void* User, const char* Section, const char* Name, const char* Value);

    static char* RStrip(char* String);

    static char* LSkip(const char* String);

    static char* FindCharOrComment(const char* String, char Char);

    static char* SafeStringCopy(char* Destination, const char* Source, size_t Size);

    static int Parse(
        const char* FileName,
        int (*handler)(void* user, const char* section, const char* name, const char* value),
        void* user);

    static int ini_parse_file(
        FILE* file,
        int (*handler)(void*, const char*, const char*, const char*),
        void* user);

    int _error;

    std::map<std::string, std::string> _values;

    std::set<std::string> _sections;

    std::map<std::string, std::set<std::string>*> _fields;
};