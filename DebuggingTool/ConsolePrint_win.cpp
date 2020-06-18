#include <Windows.h>	// for OutputDebugStringA(). Uggh.. this pulls in a lot of Windows specific stuff
#include <assert.h>		// for assert()
#include <stdarg.h>		// for va_<xxx>
#include <stdio.h>		// for vsprintf_s()
#include "ConsolePrint.h"
namespace GStar
{
	void ConsolePrint(LOGPlatform platform, LOGType type, const char * i_fmt, ...)
	{
		assert(i_fmt);
		const size_t lenTemp = 256;
		char strTemp[lenTemp] = "";
		char* PreFix = (char*)(malloc(sizeof(char) * 10));// Turn this number to 8 to see a heep curruption error

		memset(PreFix, '\0', 10);
		if (type == LOGType::Log) {
			strcpy_s(PreFix, 10, "LOG:<");
		}
		else if (type == LOGType::Waring) {
			strcpy_s(PreFix, 10, "Warning:<");
		}
		else {
			strcpy_s(PreFix, 10, "Error:<");
		}
		char PostFix[3] = ">\n";

		strcat_s(strTemp, PreFix);
		strcat_s(strTemp, i_fmt);
		strcat_s(strTemp, PostFix);
		const size_t lenOutput = lenTemp + 32;

		char strOutput[lenOutput];

		// define a variable argument list variable 
		va_list args;

		// initialize it. second parameter is variable immediately
		// preceeding variable arguments
		va_start(args, i_fmt);

		// (safely) print our formatted string to a char buffer
		vsprintf_s(strOutput, lenOutput, strTemp, args);

		va_end(args);
		if (platform == GStar::LOGPlatform::Output) {
			OutputDebugStringA(strOutput);
		}
		else {
			printf(strOutput);
		}
		ASSERT(!(type == GStar::LOGType::Error), strOutput);
		if (type == GStar::LOGType::Waring) {
			DebugBreak();
		}
		free(PreFix);
	}

} // namespace Engine