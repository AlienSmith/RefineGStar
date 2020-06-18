#pragma once
#include<iostream>
namespace GStar
{
	enum class LOGType { Log = 0, Waring = 1, Error = 2 };
    enum class  LOGPlatform{ Output = 0, Console = 1 };
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


#if defined(_DEBUG) 
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif