#include "Log.h"

#ifdef __linux__

#include <stdio.h>
#include <cstdarg>

void Log(const char* file, int line, const char* format, ...) {
    va_list args;
    va_start(args, format);

    printf("[%s:%d] ", file, line);
    vprintf(format, args);

    va_end(args);
}

#elif _WIN32

#include <windows.h>
#include <stdio.h>

void Log(const char file[], int line, const char* format, ...)
{
	static char tmpString1[4096];
	static char tmpString2[4096];
	static va_list ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmpString1, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmpString2, 4096, "\n%s(%d) : %s", file, line, tmpString1);

	OutputDebugString(tmpString2);
}
#endif