#ifndef __CONSOLE_PRINT_H
#define __CONSOLE_PRINT_H
namespace GStar
{
	enum LOGType { Log = 0, Waring = 1, Error = 2 };
	enum LOGPlatform { Output = 0, Console = 1 };
	void ConsolePrint(LOGPlatform platform, LOGType type, const char * i_fmt, ...);
} // Namespace GStar

// This is how we handle variable argument pre-processor macros
// This is only supported by compilers that support the C99 standard
#if defined(_DEBUG)  &&  !defined(DISABLE_DEBUG_PRINT)
#define DEBUG_PRINT(platform,type,fmt,...) GStar::ConsolePrint(platform,type,(fmt),__VA_ARGS__)
#else
// This is one way we create a do nothing (NO OP) macro that doesn't
// generate a compiler warning or error
#define DEBUG_PRINT(fmt,...) void(0)
#endif


#endif // __CONSOLE_PRINT_H