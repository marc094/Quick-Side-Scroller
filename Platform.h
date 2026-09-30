#ifndef __PLATFORM_H__
#define __PLATFORM_H__

// Single place for the differences between the MSVC/Windows build and everything else.

#ifdef _WIN32
	#include "SDL/include/SDL.h"
#else
	#include <cstdio>
	#include <SDL.h>
	inline void OutputDebugString(const char* message) { fputs(message, stderr); }
#endif

#endif
