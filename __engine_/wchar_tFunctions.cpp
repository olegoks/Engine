#include "wchar_tFunctions.h"
#include <stdlib.h>
#include <cstring>

wchar_t* ConvertString(const char* char_String)
{

	size_t size = strlen(char_String) + 1;
	wchar_t* wchar_tString = new wchar_t[size];

	size_t outSize;
	mbstowcs_s(&outSize, wchar_tString, size, char_String, size - 1);

	return wchar_tString;
}
