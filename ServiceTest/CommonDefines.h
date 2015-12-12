#pragma once
#include <string>
#include <sstream>

#ifdef _UNICODE
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif

#define DebugPrint(x) OutputDebugString(_T(x))
#define DebugPrint2(x1, x2) \
	{ \
	std::ostringstream stream; \
	stream << (x1) << (x2); \
	DebugPrint(stream.str().c_str()); \
	}

