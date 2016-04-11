#include "StdAfx.h"
#include "RtnResources.h"
#include "Utils.h"
#include "Logger.h"

static const std::wstring& lpAppName = L"options";
static std::wstring iniFileName;

void RtnResources::Init(const wstring& fileName) {
	iniFileName = IO::GetFilePath(fileName);
}

bool RtnResources::IsDebugLoggingEnabled() {
	wstring debug = RtnResources::GetOption(L"debug.enabled", L"false");
	return debug.compare(L"true") == 0;
}

std::wstring RtnResources::GetOption(const wstring& propertyName, const wstring& defaultValue) {
	TCHAR value[1024];
	GetPrivateProfileString(lpAppName.c_str(), propertyName.c_str(), defaultValue.c_str(),
		value, sizeof(value)/sizeof(value[0]), iniFileName.c_str());
	return value;
}

int RtnResources::GetOptionInt(const wstring& propertyName, const int defaultValue) {
	return GetPrivateProfileInt(lpAppName.c_str(), propertyName.c_str(), defaultValue, iniFileName.c_str());
}

wstring RtnResources::GetServerType() {
	wstring serverType = GetOption(L"application.server.type", L"jboss4");
	if (L"auto" == serverType) {
		LOG_DEBUG("determining server type");
		serverType = IO::GetServerTypeByUrl("/version/");
		LOG_DEBUG_W(L"server type is '" + serverType + L"'");
	}
	if (L"jboss4" != serverType && L"jboss7" != serverType) {
		throw L"Unknown server type: '" + serverType + L"'";
	}
	return serverType;
}

wstring RtnResources::GetServerVersion() {
	wstring serverVersion = GetOption(L"server.version", L"auto");
	if (L"auto" == serverVersion) {
		LOG_DEBUG("determining server version");
		serverVersion = IO::GetRunaVersionByUrl("/wfe/version");
		LOG_DEBUG_W(L"server version is '" + serverVersion + L"'");
	}
	return serverVersion;
}

wstring RtnResources::GetWebServiceURL(wstring serverType, wstring serverVersion, wstring serviceName) {
	wstring serverName = GetOption(L"server.name", L"localhost");
	wstring serverPort = GetOption(L"server.port", L"8080");
	wstring result;
	if (L"jboss4" == serverType) {
		result = L"http://" + serverName + L":" + serverPort + L"/runawfe-wfe-service-" + serverVersion + L"/" + serviceName + L"ServiceBean";
	} else {
		result = L"http://" + serverName + L":" + serverPort + L"/wfe-service-" + serverVersion + L"/" + serviceName + L"WebService/" + serviceName + L"API";
	}
	return result;
}

wstring RtnResources::GetBrowserStartURL() {
	const wstring serverName = RtnResources::GetOption(L"server.name", L"localhost");
	const wstring serverPort = RtnResources::GetOption(L"server.port", L"8080");
	const wstring loginRelativeURL = RtnResources::GetOption(L"login.relative.url", L"/login.do");
	return L"http://" + serverName + L":" + serverPort + L"/wfe" + loginRelativeURL;
}

wstring RtnResources::GetLogFile() {
	return GetOption(L"debug.log.file", L"");
}

wstring RtnResources::GetApplicationTitle() {
	return GetOption(L"application.title", L"RunaWFE tasks notifier");
}

bool RtnResources::GetUserInputLoginSilenty(){
	wstring debug = RtnResources::GetOption(L"userinput.login.silently", L"false");
	return debug.compare(L"true") == 0;
}

wstring RtnResources::GetUserInputDefaultLogin(){
	return GetOption(L"userinput.default.login", L"Administrator");
}

wstring RtnResources::GetUserInputDefaultPassword(){
	return GetOption(L"userinput.default.password", L"wf");
}

wstring RtnResources::GetAuthenticationType(){
		return GetOption(L"authentication.type", L"userinput");
}


wstring  RtnResources::GetButtonLoginText(){
	return GetOption(L"button.login", L"Enter");
}

wstring  RtnResources::GetLabelLoginText(){
	return GetOption(L"label.login ", L"Login");
}

wstring  RtnResources::GetLabelPasswordText(){
	return GetOption(L"label.password", L"Password");
}

wstring  RtnResources::GetLabelLoginTitle(){
	return GetOption(L"label.login.title", L"Please log in");
}


