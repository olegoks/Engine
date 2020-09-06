#pragma once
#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <cstdio>
#include <windows.h>
#include "wchar_tFunctions.h"
//#include "GameEngine.h"

class Console {
private:
	const char* title;

public:

	class ConsoleCreationException{};
	Console(const char* consoleTitle);
	~Console();

};

#endif