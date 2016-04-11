#pragma once

#include <string>
#include <io.h>
#include "ws/ServerAPIBindingProxy.h"
using namespace std;

#define LOG_DEBUG(message, ...)						Logger::Log(Logger::SEVERITY_DEBUG, __FILE__, __FUNCTION__, __LINE__, message, __VA_ARGS__)
#define LOG_WARN(message, ...)						Logger::Log(Logger::SEVERITY_WARNING, __FILE__, __FUNCTION__, __LINE__, message, __VA_ARGS__)
#define LOG_INFO(message, ...)						Logger::Log(Logger::SEVERITY_INFO, __FILE__, __FUNCTION__, __LINE__, message, __VA_ARGS__)
#define LOG_ERROR(message, ...)						Logger::Log(Logger::SEVERITY_FATAL, __FILE__, __FUNCTION__, __LINE__, message, __VA_ARGS__)
#define LOG_DEBUG_W(message)						Logger::Log(Logger::SEVERITY_DEBUG, __FILE__, __FUNCTION__, __LINE__, message)
#define LOG_ERROR_W(message)						Logger::Log(Logger::SEVERITY_FATAL, __FILE__, __FUNCTION__, __LINE__, message)

class Logger
{
public:
	enum SEVERITY
	{
		SEVERITY_DEBUG,
		SEVERITY_INFO,
		SEVERITY_WARNING,
		SEVERITY_FATAL
	};
	static void Init();
	static void Log(const SEVERITY severity, const char* sourceFileName, const char* functionName, long line, const char* message, ...);
	static void Log(const SEVERITY severity, const char* sourceFileName, const char* functionName, long line, wstring message);
	static void LogWebServiceError(ServerAPIBindingProxy* proxy);
private:
	static const char* GetSeverityName(const SEVERITY severity);
};

