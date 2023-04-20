#include <iostream>
//#include "..\..\PersistanceLayer\Config\Initialization\IniData.h"
#include "..\..\PersistanceLayer\Config\Initialization\IniSection.h"
#include <unordered_set>
#include <any>
#include "..\..\PersistanceLayer\Config\Initialization\External Libraries\INIReader\IniReader.h"
#include <filesystem>
#include <Windows.h>
#include "Application.h"


int main()
{
	Application App;

	std::string Yahya("Yahya");
	std::cout << std::format("{} / {}", strlen(Yahya.c_str()), Yahya.length());
	return 0;
}