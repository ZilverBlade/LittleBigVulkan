#pragma once
#include <lbv/dll.h>
#include <iostream>
#include <windows.h> // Include the windows header
static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

#define LBVLOG_ERR(fmt, ...)				\
	(SetConsoleTextAttribute(hConsole, 12));	\
	std::printf("[ERROR]\t"##fmt"\n", __VA_ARGS__);	\
	(SetConsoleTextAttribute(hConsole, 7))

#define LBVLOG_WRN(fmt, ...)				\
	(SetConsoleTextAttribute(hConsole, 14));	\
	std::printf("[WARN]\t"##fmt"\n", __VA_ARGS__);\
	(SetConsoleTextAttribute(hConsole, 7))

#define LBVLOG_INF(fmt, ...)				\
	std::printf("[INFO]\t"##fmt"\n", __VA_ARGS__)