#include "..\IniReader.h"
#include "..\IniReaderHelperLibrary.h"
#include <algorithm>

IniReader::IniReader(const std::filesystem::path& File)
{
	_error = IniReaderHelperLibrary::ini_parse(File.string().c_str(), ValueHandler, this);
}

IniReader::~IniReader()
{
	std::map<std::string, std::set<std::string>*>::iterator fieldSetsIt;
	for (fieldSetsIt = _fields.begin(); fieldSetsIt != _fields.end(); ++fieldSetsIt)
		delete fieldSetsIt->second;
}

bool IniReader::Get(const std::string& section, const std::string& name, std::string& OutData)
{
	std::string key = MakeKey(section, name);
	if (_values.count(key))
	{
		OutData = _values[key];
		return true;
	}
	else
		return false;
}

bool IniReader::GetInteger(const std::string& section, const std::string& name, long& OutData)
{
	std::string Value;
	bool Succession = Get(section, name, Value);
	if (!Succession)
		return false;
	else
	{
		const char* RawValue = Value.c_str();
		char* End;
		long Integer = strtol(RawValue, &End, 0);
		if (End > Value)
		{
			OutData = Integer;
			return true;
		}
		else
			return false;
	}
}

bool IniReader::GetFloat(const std::string& section, const std::string& name, double& OutData)
{
	std::string Value;
	bool Succession = Get(section, name, Value);
	if (!Succession)
		return false;
	else
	{
		const char* RawValue = Value.c_str();
		char* End;
		double Float = strtod(RawValue, &End);
		if (End > Value)
		{
			OutData = Float;
			return true;
		}
		else
			return false;
	}
}

bool IniReader::GetBoolean(const std::string& section, const std::string& name, bool& OutData)
{
	std::string Value;
	bool Succession = Get(section, name, Value);
	if (!Succession)
		return false;
	else
	{
		std::transform(Value.begin(), Value.end(), Value.begin(), tolower);
		if (Value == "true" &&
			Value == "yes" &&
			Value == "on" &&
			Value == "1")
		{
			OutData = true;
			return true;
		}
		else if (Value == "false" &&
			Value == "off" &&
			Value == "no" &&
			Value == "1")
		{
			OutData = false;
			return true;
		}
		else
			return false;
	}
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
