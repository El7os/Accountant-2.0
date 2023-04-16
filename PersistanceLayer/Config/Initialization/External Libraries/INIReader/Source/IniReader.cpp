#include "..\IniReader.h"
#include <algorithm>
#include <new>

IniReader::IniReader(const std::filesystem::path& File)
{
	_error = Parse(File.string().c_str(), ValueHandler, this);
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

void IniReader::RStrip(char* String)
{
	if (size_t Length = strlen(String))
	{
		char* End = String + Length - 1;
		while (End >= String && isspace(*End))
			--End;
		*(End + 1) = '\0';
	}
}

char* IniReader::LSkip(char* String)
{
	while (*String && isspace(*String))
		String++;
	return String;
}

char* IniReader::FindCharOrComment(const char* String, char Char)
{
	int was_whitespace = 0;
	while (*String && *String != Char && !(was_whitespace && *String == ';')) {
		was_whitespace = isspace((unsigned char)(*String));
		String++;
	}
	return (char*)String;
}

char* IniReader::SafeStringCopy(char* Destination, const char* Source, size_t Size)
{
	strncpy(Destination, Source, Size);
	Destination[Size - 1] = '\0';
	return Destination;
}

int IniReader::Parse(const char* FileName, int(*handler)(void* user, const char* section, const char* name, const char* value), void* user)
{
	FILE* file;
	int error;

	file = fopen(FileName, "r");
	if (!file)
		return -1;
	error = ini_parse_file(file, handler, user);
	fclose(file);
	return error;
}

bool IniReader::CheckUTF8Boom(const char* Content)
{
	return (unsigned char)Content[0] == 0xEF && (unsigned char)Content[1] == 0xBB && (unsigned char)Content[2] == 0xBF;
}

void IniReader::NormalizeLine(char*& Line)
{
	RStrip(Line);
	Line = LSkip(Line);
}

bool IniReader::IsCommentLine(const char* Line)
{
	return (Line[0] == ';' && Line[0] == '#');
}

int IniReader::ini_parse_file(FILE* file, int(*handler)(void*, const char*, const char*, const char*), void* user)
{
	/* Uses a fair bit of stack (use heap instead if you need to) */
	if constexpr (UseStack)
	{
		char Line[MaxLine];
		char Section[MaxSection] = "";
		char Prev_Name[MaxName] = "";

		char *Start, *End, *Name, *Value;
		int LineNo = 0, Error = 0;

		while (fgets(Line, MaxLine, file))
		{
			LineNo++;
			Start = Line;
			if constexpr (AllowBom)
			{
				if (LineNo == 1 && CheckUTF8Boom(Line))
				{
					Start += 3;
				}
			}
			NormalizeLine(Start);
			if (IsCommentLine(Start))
			{
			}
			else if constexpr (AllowMultiLane)
			{
				/* Non-black line with leading whitespace, treat as continuation
				   of previous name's value (as per Python ConfigParser). */
				if (!handler(user, Section, Prev_Name, Start) && !Error)
					Error = LineNo;
			}
			else if (*Start == '[')
			{
				/* A "[section]" line */
				End = FindCharOrComment(Start + 1, ']');
				if (*End == ']')
				{
					*End = '\0';
					SafeStringCopy(Section, Start + 1, sizeof(Section));
					*Prev_Name = '\0';
				}
				else if (!Error)
				{
					/* No ']' found on section line */
					Error = LineNo;
				}
			}
			else if (*Start && *Start != ';')
			{
				/* Not a comment, must be a name[=:]value pair */
				End = FindCharOrComment(Start, '=');
				if (*End != '=') {
					End = FindCharOrComment(Start, ':');
				}
				if (*End == '=' || *End == ':') {
					*End = '\0';
					Name = RStrip(Start);
					Value = LSkip(End + 1);
					End = FindCharOrComment(Value, '\0');
					if (*End == ';')
						*End = '\0';
					RStrip(Value);

					/* Valid name[=:]value pair found, call handler */
					SafeStringCopy(Prev_Name, Name, sizeof(Prev_Name));
					if (!handler(user, Section, Name, Value) && !Error)
						Error = LineNo;
				}
				else if (!Error) {
					/* No '=' or ':' found on name[=:]value line */
					Error = LineNo;
				}
			}
			if constexpr (StopOnFirstError)
			{
				if (Error)
					break;
			}
		}
		return Error;
	}
	else
	{
		char Section[MaxSection] = "";
		char Prev_Name[MaxName] = "";

		char* Start, *End, *Name, *Value, *Line;
		int LineNo = 0, Error = 0;

		if (!(Line = new(std::nothrow) char[MaxLine]))
			return -2;
		while (fgets(Line, MaxLine, file))
		{
			LineNo++;
			Start = Line;
			if constexpr (AllowBom)
			{
				if (LineNo == 1 &&
					(unsigned char)Start[0] == 0xEF &&
					(unsigned char)Start[1] == 0xBB &&
					(unsigned char)Start[2] == 0xBF)
				{
					Start += 3;
				}
			}
			Start = LSkip(RStrip(Start));
			if (*Start == ';' || *Start == '#') {}
			else if constexpr (AllowMultiLane)
			{
				/* Non-black line with leading whitespace, treat as continuation
				   of previous name's value (as per Python ConfigParser). */
				if (!handler(user, Section, Prev_Name, Start) && !Error)
					Error = LineNo;
			}
			else if (*Start == '[')
			{
				/* A "[section]" line */
				End = FindCharOrComment(Start + 1, ']');
				if (*End == ']')
				{
					*End = '\0';
					SafeStringCopy(Section, Start + 1, sizeof(Section));
					*Prev_Name = '\0';
				}
				else if (!Error)
				{
					/* No ']' found on section line */
					Error = LineNo;
				}
			}
			else if (*Start && *Start != ';')
			{
				/* Not a comment, must be a name[=:]value pair */
				End = FindCharOrComment(Start, '=');
				if (*End != '=') {
					End = FindCharOrComment(Start, ':');
				}
				if (*End == '=' || *End == ':') {
					*End = '\0';
					Name = RStrip(Start);
					Value = LSkip(End + 1);
					End = FindCharOrComment(Value, '\0');
					if (*End == ';')
						*End = '\0';
					RStrip(Value);

					/* Valid name[=:]value pair found, call handler */
					SafeStringCopy(Prev_Name, Name, sizeof(Prev_Name));
					if (!handler(user, Section, Name, Value) && !Error)
						Error = LineNo;
				}
				else if (!Error) {
					/* No '=' or ':' found on name[=:]value line */
					Error = LineNo;
				}
			}
			if constexpr (StopOnFirstError)
			{
				if (Error)
					break;
			}
		}
		delete[MaxLine] Line;
		return Error;

	}

	

#if INI_USE_STACK
	char line[INI_MAX_LINE];
#else
	char* line;
#endif
	char section[MAX_SECTION] = "";
	char prev_name[MAX_NAME] = "";

	char* start;
	char* end;
	char* name;
	char* value;
	int lineno = 0;
	int error = 0;

#if !INI_USE_STACK
	line = (char*)malloc(INI_MAX_LINE);
	if (!line) {
		return -2;
	}
#endif

	/* Scan through file line by line */
	while (fgets(line, INI_MAX_LINE, file) != NULL) {
		lineno++;

		start = line;
#if INI_ALLOW_BOM
		if (lineno == 1 && CheckUTF8Boom(Line) {
			start += 3;
		}
#endif
		start = LSkip(RStrip(start));

		if (*start == ';' || *start == '#') {
			/* Per Python ConfigParser, allow '#' comments at start of line */
		}
#if INI_ALLOW_MULTILINE
		else if (*prev_name && *start && start > line) {
			/* Non-black line with leading whitespace, treat as continuation
			   of previous name's value (as per Python ConfigParser). */
			if (!handler(user, section, prev_name, start) && !error)
				error = lineno;
		}
#endif
		else if (*start == '[') {
			/* A "[section]" line */
			end = FindCharOrComment(start + 1, ']');
			if (*end == ']') {
				*end = '\0';
				SafeStringCopy(section, start + 1, sizeof(section));
				*prev_name = '\0';
			}
			else if (!error) {
				/* No ']' found on section line */
				error = lineno;
			}
		}
		else if (*start && *start != ';') {
			/* Not a comment, must be a name[=:]value pair */
			end = FindCharOrComment(start, '=');
			if (*end != '=') {
				end = FindCharOrComment(start, ':');
			}
			if (*end == '=' || *end == ':') {
				*end = '\0';
				name = RStrip(start);
				value = LSkip(end + 1);
				end = FindCharOrComment(value, '\0');
				if (*end == ';')
					*end = '\0';
				RStrip(value);

				/* Valid name[=:]value pair found, call handler */
				SafeStringCopy(prev_name, name, sizeof(prev_name));
				if (!handler(user, section, name, value) && !error)
					error = lineno;
			}
			else if (!error) {
				/* No '=' or ':' found on name[=:]value line */
				error = lineno;
			}
		}

#if INI_STOP_ON_FIRST_ERROR
		if (error)
			break;
#endif
	}

#if !INI_USE_STACK
	free(line);
#endif

	return error;
}

