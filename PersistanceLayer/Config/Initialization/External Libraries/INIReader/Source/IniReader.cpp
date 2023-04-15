#include "..\IniReader.h"
#include "..\IniReaderHelperLibrary.h"
#include <algorithm>

IniReader::IniReader(const std::string& filename)
{
	_error = IniReaderHelperLibrary::ini_parse(filename.c_str(), ValueHandler, this);
}

IniReader::~IniReader()
{
	std::map<std::string, std::set<std::string>*>::iterator fieldSetsIt;
	for (fieldSetsIt = _fields.begin(); fieldSetsIt != _fields.end(); ++fieldSetsIt)
		delete fieldSetsIt->second;
}

std::string IniReader::Get(const std::string& section, const std::string& name, const std::string& default_value)
{
	std::string key = MakeKey(section, name);
	return _values.count(key) ? _values[key] : default_value;
}

long IniReader::GetInteger(const std::string& section, const std::string& name, long default_value)
{
	std::string valstr = Get(section, name, "");
	const char* value = valstr.c_str();
	char* end;
	
	long n = strtol(value, &end, 0);
	return end > value ? n : default_value;
}

double IniReader::GetReal(const std::string& section, const std::string& name, double default_value)
{
	std::string valstr = Get(section, name, "");
	const char* value = valstr.c_str();
	char* end;
	double n = strtod(value, &end);
	return end > value ? n : default_value;
}

bool IniReader::GetBoolean(const std::string& section, const std::string& name, bool default_value)
{
	std::string valstr = Get(section, name, "");
	// Convert to lower case to make string comparisons case-insensitive
	std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
	if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
		return true;
	else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
		return false;
	else
		return default_value;
}

std::set<std::string> IniReader::GetFields(const std::string& section) const
{
	std::string sectionKey = section;
	std::transform(sectionKey.begin(), sectionKey.end(), sectionKey.begin(), ::tolower);
	std::map<std::string, std::set<std::string>*>::const_iterator fieldSetIt = _fields.find(sectionKey);
	if (fieldSetIt == _fields.end())
		return std::set<std::string>();
	return *(fieldSetIt->second);
}

std::string IniReader::MakeKey(const std::string& section, const std::string& name)
{
	std::string key = section + "=" + name;
	// Convert to lower case to make section/name lookups case-insensitive
	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
	return key;
}

int IniReader::ValueHandler(void* user, const char* section, const char* name, const char* value)
{
	IniReader* reader = (IniReader*)user;

	// Add the value to the lookup map
	std::string key = MakeKey(section, name);
	if (reader->_values[key].size() > 0)
		reader->_values[key] += "\n";
	reader->_values[key] += value;

	// Insert the section in the sections set
	reader->_sections.insert(section);

	// Add the value to the values set
	std::string sectionKey = section;
	std::transform(sectionKey.begin(), sectionKey.end(), sectionKey.begin(), ::tolower);

	std::set<std::string>* fieldsSet;
	std::map<std::string, std::set<std::string>*>::iterator fieldSetIt = reader->_fields.find(sectionKey);
	if (fieldSetIt == reader->_fields.end())
	{
		fieldsSet = new std::set<std::string>();
		reader->_fields.insert(std::pair<std::string, std::set<std::string>*>(sectionKey, fieldsSet));
	}
	else {
		fieldsSet = fieldSetIt->second;
	}
	fieldsSet->insert(name);

	return 1;
}
