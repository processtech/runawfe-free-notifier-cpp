// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
//#ifdef _DEBUG
//   #ifndef DBG_NEW
//      #define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//      #define new DBG_NEW
//   #endif
//#endif  // _DEBUG

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN

//#define WINHTTP_WRAPPER_NO_COMPRESSION_SUPPORT
//#define OMIT_PROFILER

//------------------------------------------------------

#include <windows.h>
#include <tchar.h>

#include <vector>
#include <string>
//#include <locale>
//#include <codecvt>

//#include <Shlwapi.h>	// for CoInitializeEx
#include <shellapi.h>	// for NOTIFYICONDATA
//#include <tlhelp32.h>

#include <winsock2.h>
#include <iostream>
#pragma comment(lib,"ws2_32.lib")

//------------------------------------------------------

#define HR(expr) { hr = expr; if (FAILED(hr)) return hr; }
