#pragma once
#include <string>
#include <set>
#include <map>

//Read an INI file into easy-to-access name/value pairs. (Note that I've gone
// for simplicity here rather than speed, but it should be pretty decent.)
class IniReader
{
public:

    // Construct INIReader and parse given filename. See ini.h for more info
    // about the parsing.
    IniReader(const std::string& filename);

    ~IniReader();

    // Return the result of ini_parse(), i.e., 0 on success, line number of
    // first error on parse error, or -1 on file open error.
    inline int ParseError() const
    {
        return _error;
    }

    // Get a string value from INI file, returning default_value if not found.
    std::string Get(const std::string& section, const std::string& name, const std::string& default_value);

    // Get an integer (long) value from INI file, returning default_value if
    // not found or not a valid integer (decimal "1234", "-1234", or hex "0x4d2").
    long GetInteger(const std::string& section, const std::string& name, long default_value);

    // Get a real (floating point double) value from INI file, returning
    // default_value if not found or not a valid floating point value
    // according to strtod().
    double GetReal(const std::string& section, const std::string& name, double default_value);

    // Get a boolean value from INI file, returning default_value if not found or if
    // not a valid true/false value. Valid true values are "true", "yes", "on", "1",
    // and valid false values are "false", "no", "off", "0" (not case sensitive).
    bool GetBoolean(const std::string& section, const std::string& name, bool default_value);

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