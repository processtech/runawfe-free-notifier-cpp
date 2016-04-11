#pragma once

// The following macros define the minimum required platform.  The minimum required platform
// is the earliest version of Windows, Internet Explorer etc. that has the necessary features to run 
// your application.  The macros work by enabling all features available on platform versions up to and 
// including the version specified.

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
// #ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
// #define _WIN32_WINNT	_WIN32_WINNT_WIN2K     // Change this to the appropriate value to target other versions of Windows.
// #endif
// 
// #ifndef _WIN32_IE
// #define _WIN32_IE		_WIN32_IE_WIN2K
// #endif
// 
// #ifndef NTDDI_VERSION
// #define NTDDI_VERSION	NTDDI_WIN2K
// #endif

#define NTDDI_VERSION	NTDDI_WINXP
#define _WIN32_WINNT	_WIN32_WINNT_WINXP
#define _WIN32_IE		_WIN32_IE_WIN2KSP3

// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#include <SDKDDKVer.h>
