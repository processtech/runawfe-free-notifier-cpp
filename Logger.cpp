#include "StdAfx.h"
#include "Logger.h"
#include "Utils.h"
#include "RtnResources.h"

static FILE* logFile;
static const char* delimiter = "\r\n";
static int logLevel;

void Logger::Init() {
	wstring logFileName = IO::GetTemporaryFilePath(L"rtn.log");
	logFile = _wfopen(logFileName.c_str(), L"ab+");
	if (logFile == NULL) {
		exit(301);
	}
	logLevel = RtnResources::IsDebugLoggingEnabled() ? SEVERITY_DEBUG : SEVERITY_INFO;
}

const char* Logger::GetSeverityName(const SEVERITY severity) {
	switch(severity) {
	case SEVERITY_DEBUG:
		return "DEBUG";
	case SEVERITY_INFO:
		return "INFO";
	case SEVERITY_WARNING:
		return "WARNING";
	case SEVERITY_FATAL:
		return "ERROR";
	}
	return "NONE";
}

void Logger::Log(const SEVERITY severity, const char* sourceFileName, const char* functionName, long line, wstring message) {
	string s = IO::ToString(message);
	Log(severity, sourceFileName, functionName, line, s.c_str());
}

void Logger::Log(const SEVERITY severity, const char* sourceFileName, const char* functionName, long line, const char* message, ...) {
	if (severity < logLevel) {
		return;
	}
	SYSTEMTIME st;
	memset(&st, 0, sizeof(st));
	GetLocalTime(&st);
	fprintf(logFile, "%04d-%02d-%02d %02d:%02d:%02d\t", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	fprintf(logFile, "%s\t", GetSeverityName(severity));
	fprintf(logFile, "%s\t%s:%d\t", sourceFileName, functionName, line);
	va_list arglist;
	va_start(arglist, message);
	char buffer[1024];
	vsprintf(buffer, message, arglist);
	fprintf(logFile, buffer);
	va_end(arglist);
	fprintf(logFile, delimiter);
	if (severity == SEVERITY_FATAL) {
		fprintf(logFile, "GetLastError() = %d", GetLastError());
		fprintf(logFile, delimiter);
	}
	fflush(logFile);
	//MessageBox(hWnd, L"Error loading menu.", GetErrorMessageBoxTitle().c_str(), MB_OK | MB_ICONERROR);
}

void Logger::LogWebServiceError(ServerAPIBindingProxy* proxy) {
    LOG_ERROR("WebService invocation error. See details in rtn.webservice.error.log");
	wstring logFileName = IO::GetTemporaryFilePath(L"rtn.webservice.error.log");
	FILE* logFile = _wfopen(logFileName.c_str(), L"ab+");
	proxy->soap_print_fault(logFile);
	fclose(logFile);
}