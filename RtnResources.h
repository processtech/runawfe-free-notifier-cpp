#pragma once
#include <string>
using namespace std;

class RtnResources {
public:
	static void Init(const wstring& fileName);
	static bool IsDebugLoggingEnabled();
	static wstring GetOption(const wstring& propertyName, const wstring& defaultValue);
	static string GetOption(const string& propertyName, const string& defaultValue);
	static int GetOptionInt(const wstring& propertyName, const int defaultValue);
	static wstring GetServerType();
	static wstring GetServerVersion();
	static wstring GetWebServiceURL(wstring serverType, wstring serverVersion, wstring serviceName);
	static wstring GetBrowserStartURL();
	static wstring GetLogFile();
	static wstring GetApplicationTitle();

	/**
	* Ask user for login and password or use default
	*/
	static bool GetUserInputLoginSilenty();
	
	/**
	* User default login
	*/
	static wstring GetUserInputDefaultLogin();

	/**
	* User default password
	*/
	static wstring GetUserInputDefaultPassword();

	/**
	* Get auth type that will be used by client
	*/
	static wstring GetAuthenticationType();

	static wstring GetButtonLoginText();

	static wstring GetLabelLoginText();

	static wstring GetLabelPasswordText();

	static wstring GetLabelLoginTitle();
};

