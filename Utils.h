#pragma once
#include <string>
#include "RtnResources.h"
#include "Connector.h"
#define WM_CUSTOM_NOTIFY_ICON	(WM_APP + 1)
#define BASIC_AUTH L"userinput"
using namespace std;

namespace Auth {
	string GetKerberosTicket(const wstring& sKerberosTargetName);
}

namespace IO {
	wstring ToWideString(const string& string);
	string ToString(const wstring& string);
	wstring GetFilePath(const wstring& relativeFileName);
	wstring GetTemporaryFilePath(const wstring& relativeFileName);
	wstring GetRunaVersionByUrl(const string& url);
	wstring GetServerTypeByUrl(const string& url);
	string UriEncode(const string & sSrc);
}

namespace UI {
	void QuitApplication(HWND hWnd, Server::Connector* connector, const char* cause);
	void LaunchBrowser(HWND hWnd);
	void LaunchBrowser(HWND hWnd, wstring login, wstring password); 
	void SetMenuLabel(HMENU hMenu, int nID, const wstring& label);
	bool UpdateNotificationIcon(HWND hWnd, Server::Connector* connector, bool modeAdd = false);
	int DeleteNotificationIcon(HWND hWnd);
}
