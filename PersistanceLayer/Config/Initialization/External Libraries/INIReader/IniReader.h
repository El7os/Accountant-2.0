#pragma once
#include <string>
#include <set>
#include <map>
#include <filesystem>


/*Reads an .ini file into easy-to-access name/value pairs.*/
class IniReader
{
public:

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

    // Get a string value from INI file, returning default_value if not found.
    bool Get(const std::string& section, const std::string& name, std::string& OutData);

    // Get an integer (long) value from INI file, returning default_value if
    // not found or not a valid integer (decimal "1234", "-1234", or hex "0x4d2").
    bool GetInteger(const std::string& section, const std::string& name, long& OutData);

    // Get a real (floating point double) value from INI file, returning
    // default_value if not found or not a valid floating point value
    // according to strtod().
    bool GetFloat(const std::string& section, const std::string& name, double& OutData);

    // Get a boolean value from INI file, returning default_value if not found or if
    // not a valid true/false value. Valid true values are "true", "yes", "on", "1",
    // and valid false values are "false", "no", "off", "0" (not case sensitive).
    bool GetBoolean(const std::string& section, const std::string& name, bool& OutData);

    // Returns all the section names from the INI file, in alphabetical order, but in the
    // original casing
    inline const std::set<std::string>& GetSections() const
    {
        return _sections;
    }


    // Returns all the field names from a section in the INI file, in alphabetical order,
    // but in the original casing. Returns an empty set if the field name is unknown
    std::set<std::string> GetFields(const std::string& section) const;

private:

    static std::string MakeKey(const std::string& section, const std::string& name);

    static int ValueHandler(void* user, const char* section, const char* name, const char* value);

    int _error;

    std::map<std::string, std::string> _values;

    std::set<std::string> _sections;

    std::map<std::string, std::set<std::string>*> _fields;
};