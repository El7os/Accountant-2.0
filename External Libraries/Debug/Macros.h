#pragma once
#define DEBUG 1

#ifdef DEBUG

#include <windows.h>
#include <iostream>
#include <format>

#define LOG(LogLevel, Text, ...)																										\
{																																		\
	HANDLE ConsoleHandler;																												\
	ConsoleHandler = GetStdHandle(STD_OUTPUT_HANDLE);																				    \
    char Color;																															\
	Color = !strcmp(#LogLevel,"Display") ? 10 : !strcmp(#LogLevel,"Error") ? 12 : 14;													\
	SetConsoleTextAttribute(ConsoleHandler,Color);																						\
	if (Color == 14 && strcmp(#LogLevel,"Warning"))																							\
	{																																	\
		std::cout << std::format("\nInvalid Log Level Attempted ({})({})", __FILE__,__LINE__) << std::endl;								\
		SetConsoleTextAttribute(ConsoleHandler, 15);																					\
	}																																	\
	else																																\
	{																																	\
		std::cout << std::format("\n[{}][{}]", #LogLevel, __LINE__);																	\
		std::cout << std::format(Text, __VA_ARGS__) << std::endl;																		\
		SetConsoleTextAttribute(ConsoleHandler, 15);                                                                                    \
	}																																	\
}																																																

#else
#define LOG(LogLevel, TEXT, ...)
#endif // 