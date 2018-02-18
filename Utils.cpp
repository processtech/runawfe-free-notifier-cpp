#include "StdAfx.h"
#include "Utils.h"
#include "Logger.h"
#include "RtnResources.h"

#define SECURITY_WIN32
#define WIN32_CHICAGO
#include <Security.h>
#pragma comment (lib, "Secur32.lib")

#ifndef _GSSAPI_H_
typedef unsigned long gss_uint32;
typedef gss_uint32      OM_uint32;
#endif


const char SAFE[256] =
{
    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
    /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,
    
    /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    
    /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    
    /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

const char HEX2DEC[256] = 
{
    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
    /* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,
    
    /* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    
    /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    
    /* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
};

int EstablishSecurityContext(wchar_t *service_name, OM_uint32 deleg_flag, CtxtHandle *gss_context, OM_uint32 *ret_flags, OUT string& Token) {
	SecBuffer sendToken, recv_tok;
	SecBufferDesc input_desc, output_desc;
	OM_uint32 maj_stat;
	CredHandle cred_handle;
	TimeStamp expiry;
	PCtxtHandle context_handle = NULL;
	input_desc.cBuffers = 1;
	input_desc.pBuffers = &recv_tok;
	input_desc.ulVersion = SECBUFFER_VERSION;
	recv_tok.BufferType = SECBUFFER_TOKEN;
	recv_tok.cbBuffer = 0;
	recv_tok.pvBuffer = NULL;
	output_desc.cBuffers = 1;
	output_desc.pBuffers = &sendToken;
	output_desc.ulVersion = SECBUFFER_VERSION;
	sendToken.BufferType = SECBUFFER_TOKEN;
	sendToken.cbBuffer = 0;
	sendToken.pvBuffer = NULL;
	cred_handle.dwLower = 0;
	cred_handle.dwUpper = 0;
	maj_stat = AcquireCredentialsHandle(
									NULL,						// no principal name
									L"Kerberos",				// package name
									SECPKG_CRED_OUTBOUND,
									NULL,						// no logon id
									NULL,						// no auth data
									NULL,						// no get key fn
									NULL,						// noget key arg
									&cred_handle,
									&expiry
									);
	if (maj_stat != SEC_E_OK) {
		LOG_ERROR("acquiring credentials failed: 0x%08X (%d)", maj_stat, maj_stat);
		return -1;
	}
	gss_context->dwLower = 0;
	gss_context->dwUpper = 0;
	do
	{
		maj_stat = InitializeSecurityContext(
							&cred_handle,
							context_handle,
							service_name,
							deleg_flag,
							0,			// reserved
							SECURITY_NATIVE_DREP,
							&input_desc,
							0,			// reserved
							gss_context,
							&output_desc,
							ret_flags,
							&expiry
							);
		if (recv_tok.pvBuffer) {
			free(recv_tok.pvBuffer);
			recv_tok.pvBuffer = NULL;
			recv_tok.cbBuffer = 0;
		}
		context_handle = gss_context;
		if (maj_stat!=SEC_E_OK && maj_stat!=SEC_I_CONTINUE_NEEDED) {
			LOG_ERROR("initializing context failed: 0x%08X (%d)", maj_stat, maj_stat);
			FreeCredentialsHandle(&cred_handle);
			return -1;
		}
		if (sendToken.cbBuffer > 0) {
			LOG_INFO("init_sec_context returned token (size=%d)", sendToken.cbBuffer);
			Token = string((const char*)sendToken.pvBuffer, sendToken.cbBuffer);
		}
		FreeContextBuffer(sendToken.pvBuffer);
		sendToken.pvBuffer = NULL;
		sendToken.cbBuffer = 0;
		if (maj_stat == SEC_I_CONTINUE_NEEDED) {
			LOG_WARN("continue needed...");
			break;
		}
	} while (maj_stat == SEC_I_CONTINUE_NEEDED);
	FreeCredentialsHandle(&cred_handle);
	return 0;
}

string Auth::GetKerberosTicket(const wstring& sKerberosTargetName) {
	string Token;
	SECURITY_STATUS maj_stat = 0;
	unsigned long sspi_ret_flags = 0;
	CtxtHandle context;
	ZeroMemory(&context, sizeof(context));
	WCHAR lpTargetName[1024] = L"";
	wcscpy_s(lpTargetName, sKerberosTargetName.c_str());
	maj_stat = EstablishSecurityContext(
		lpTargetName,
		//ISC_REQ_MUTUAL_AUTH |
		ISC_REQ_ALLOCATE_MEMORY |
		ISC_REQ_CONFIDENTIALITY |
		//ISC_REQ_REPLAY_DETECT |
		//ISC_REQ_IDENTIFY |
		//ISC_REQ_DATAGRAM |
		0
		,
		&context,
		&sspi_ret_flags,
		Token
		);
	if (!SEC_SUCCESS(maj_stat)) {
		LOG_ERROR("Establishing security context failed!\n");
		return "";
	}
	LOG_DEBUG("Kerberos ticket: %d bytes length", Token.size());
	DeleteSecurityContext (&context);
	return Token;
}

wstring IO::ToWideString(const string& mbString) {
	//wchar_t* chars = new wchar_t[mbString.length()];
	//mbstowcs(chars, mbString.c_str(), wcslen(chars));
	//return wstring(chars);	
	return wstring(mbString.begin(), mbString.end());
}

string IO::ToString(const wstring& wideString) {
	//char* chars = new char[wideString.length()];
	//wcstombs(chars, wideString.c_str(), strlen(chars));
	//return string(chars);
	//
	//wstring_convert<codecvt_utf8<wchar_t>> converter;
	//return converter.to_bytes(wideString);
	return string(wideString.begin(), wideString.end());
}

wstring IO::GetFilePath(const wstring& relativeFileName) {
	wchar_t modulePath[MAX_PATH] = _T("");
	GetModuleFileName(NULL, modulePath, MAX_PATH);
	wstring filename(modulePath);
	filename = filename.substr(0, filename.length() - 7);
	//PathRemoveFileSpec(modulePath);
	//wchar_t path[MAX_PATH] = _T("");
	//if(!PathCombine(path, modulePath, relativeFileName.c_str())) {
	//	throw new exception("Unable to GetFilePath");
	//}
	return filename + relativeFileName;
}

wstring IO::GetTemporaryFilePath(const wstring& relativeFileName) {
	wchar_t tempDirectoryPath[MAX_PATH] = _T("");
	GetTempPath(MAX_PATH, tempDirectoryPath);
	//TCHAR path[MAX_PATH] = _T("");
	//if(!PathCombine(path, tempDirectoryPath, relativeFileName.c_str())) {
	//	throw new exception("Unable to GetTemporaryFilePath");
	//}
	return wstring(tempDirectoryPath) + relativeFileName;
}

bool recvline(int s, string* buf) {
	char in;
	int t = 0;
	while (true) {
		int available = recv(s, &in, 1, 0);
		if (available <= 0) {
			return false; 
		}
		if (in == 10) {
			continue; 
		}
		if (in == 13) {
			return true; 
		}
		*buf += in;
    }
}

// TODO? http://www.rsdn.ru/article/inet/wininet.xml
wstring IO::GetRunaVersionByUrl(const string& url) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        throw "WSAStartup failed";
    }
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	string serverName = ToString(RtnResources::GetOption(L"server.name", L""));
	struct hostent *host = gethostbyname(serverName.c_str());
	if (host == NULL) {
		throw "Could not find host by name '" + serverName + "'";
	}
    SOCKADDR_IN socketAddress;
	socketAddress.sin_port = htons(RtnResources::GetOptionInt(L"server.port", 80));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = *((unsigned long*) host->h_addr);
	int resultCode = connect(clientSocket, (SOCKADDR*) &socketAddress, sizeof(socketAddress));
    if (resultCode != 0) {
		LOG_ERROR("Could not connect to %s, resultCode = %d", serverName.c_str(), resultCode);
        throw "Could not connect to '" + serverName + "'";
	}
	string request = "GET " + url + " HTTP/1.1\r\nHost: " + serverName + "\r\nConnection: close\r\n\r\n";
	send(clientSocket, request.c_str(), request.length(), 0);
	string buffer;
	string version;
	bool readResponse = true;
	bool nextVersion=false;
	while (readResponse) {		
		readResponse = recvline(clientSocket, &buffer);
		if(nextVersion){
			if(!buffer.empty()){
				int versionSt= buffer.find(":");
				if(versionSt >-1 ){
					version = buffer.substr(versionSt);
				}else{
					version = buffer;
				}
			}
		}
		if(buffer.length() == 0){
			nextVersion = true;
		}		
		buffer.clear();
	}
    closesocket(clientSocket);
    WSACleanup();
	if (!version.empty()) {
		return ToWideString(version);
	}	
	string message = "No version found by URL '" + url + "'";
    throw message;
}

wstring IO::GetServerTypeByUrl(const string& url) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        throw "WSAStartup failed";
    }
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	string serverName = ToString(RtnResources::GetOption(L"server.name", L""));
	struct hostent *host = gethostbyname(serverName.c_str());
	if (host == NULL) {
		throw "Could not find host by name '" + serverName + "'";
	}
    SOCKADDR_IN socketAddress;
	socketAddress.sin_port = htons(RtnResources::GetOptionInt(L"server.port", 80));
    socketAddress.sin_family = AF_INET;
    socketAddress.sin_addr.s_addr = *((unsigned long*) host->h_addr);
	int resultCode = connect(clientSocket, (SOCKADDR*) &socketAddress, sizeof(socketAddress));
    if (resultCode != 0) {
		LOG_ERROR("Could not connect to %s, resultCode = %d", serverName.c_str(), resultCode);
        throw "Could not connect to '" + serverName + "'";
	}
	string request = "GET " + url + " HTTP/1.1\r\nHost: " + serverName + "\r\nConnection: close\r\n\r\n";
	send(clientSocket, request.c_str(), request.length(), 0);
	string buffer;
	string version;
	bool readResponse = true;
	bool nextVersion=false;
	while (readResponse) {
		readResponse = recvline(clientSocket, &buffer);
		if (buffer.find("VERSION:") == 0) {
			version = buffer.substr(8, buffer.length() - 8);
		}
		buffer.clear();
	}
    closesocket(clientSocket);
    WSACleanup();
	if (version.empty()) {
		version="jboss7";
	}	
	if (!version.empty()) {
		return ToWideString(version);
	}	
	string message = "No version found by URL '" + url + "'";
    throw message;
}



std::string IO::UriEncode(const string & sSrc)
{
   const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
   const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
   const int SRC_LEN = sSrc.length();
   unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
   unsigned char * pEnd = pStart;
   const unsigned char * const SRC_END = pSrc + SRC_LEN;

   for (; pSrc < SRC_END; ++pSrc)
   {
      if (SAFE[*pSrc]) 
         *pEnd++ = *pSrc;
      else
      {
         // escape this char
         *pEnd++ = '%';
         *pEnd++ = DEC2HEX[*pSrc >> 4];
         *pEnd++ = DEC2HEX[*pSrc & 0x0F];
      }
   }

   std::string sResult((char *)pStart, (char *)pEnd);   
   delete [] pStart;
   return sResult;
}



void UI::QuitApplication(HWND hWnd, Server::Connector* connector, const char* cause) {
	LOG_DEBUG("QuitApplication, cause: %s", cause);
	DestroyWindow(hWnd);
	delete connector;
	_CrtDumpMemoryLeaks();
}

void UI::LaunchBrowser(HWND hWnd) {
	LOG_DEBUG("LaunchBrowser");
	// use empty string to use default system browser, L"IEXPLORE.EXE" for MSIE:
	const wstring command = RtnResources::GetOption(L"browser.command", L"");
	const int show = RtnResources::GetOptionInt(L"browser.command.show", SW_SHOWMAXIMIZED);
	ShellExecute(hWnd, L"open", command.c_str(), RtnResources::GetBrowserStartURL().c_str(), NULL, show);
}

std::string wstring_to_utf8_hex(const std::wstring &input)
{
  std::string output;
  int cbNeeded = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, NULL, 0, NULL, NULL);
  if (cbNeeded > 0) {
    char *utf8 = new char[cbNeeded];
    if (WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, utf8, cbNeeded, NULL, NULL) != 0) {
      for (char *p = utf8; *p; *p++) {
        char onehex[5];
        _snprintf(onehex, sizeof(onehex), "%%%02.2X", (unsigned char)*p);
        output.append(onehex);
      }
    }
    delete[] utf8;
  }
  return output;
}

void UI::LaunchBrowser(HWND hWnd, wstring login, wstring password) {
	LOG_DEBUG("LaunchBrowser with login and password");
	// use empty string to use default system browser, L"IEXPLORE.EXE" for MSIE:
	const wstring command = RtnResources::GetOption(L"browser.command", L"");
	const int show = RtnResources::GetOptionInt(L"browser.command.show", SW_SHOWMAXIMIZED);
	string ePassword= wstring_to_utf8_hex(password);		
	string eLogin =  wstring_to_utf8_hex(login);
	wstring tPassword = IO::ToWideString(ePassword);
	wstring tLogin = IO::ToWideString(eLogin);
	wstring path =L"login="+tLogin+L"&"+L"password="+tPassword; 	
	wstring url = RtnResources::GetBrowserStartURL();
	url+=L"?"+path;
	ShellExecute(hWnd, L"open", command.c_str(), url.c_str(), NULL, show);
}

void UI::SetMenuLabel(HMENU hMenu, int nID, const wstring& label) {
	ModifyMenu(hMenu, nID, MF_BYCOMMAND | MF_STRING, nID, label.c_str());
}

bool UI::UpdateNotificationIcon(HWND hWnd, Server::Connector* connector, bool modeAdd) {
	if (!modeAdd && !connector->HasChanges()) {
		return false;
	}
	if (modeAdd) {
		int popupTimeout = RtnResources::GetOptionInt(L"popup.timeout", 5);
		if (popupTimeout != 5) {
			// SPI_SETMESSAGEDURATION
			SystemParametersInfo(0x2017, 0, IntToPtr(35), 0);
		}
	}
	NOTIFYICONDATAW nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = NOTIFYICONDATAW_V2_SIZE;
	nid.hWnd = hWnd;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	nid.uCallbackMessage = WM_CUSTOM_NOTIFY_ICON;
	nid.hIcon = connector->GetNotificationIcon();
	if (!nid.hIcon) {
		return false;
	}
	wstring tooltip = connector->GetNotificationTooltipMessage();
	wcscpy_s(nid.szTip, tooltip.c_str());
	if (connector->HasNotificationAboutNewTasks()) {
		nid.dwInfoFlags = NIIF_INFO;
		wcscpy_s(nid.szInfoTitle, RtnResources::GetApplicationTitle().c_str());
		wcscpy_s(nid.szInfo, connector->GetNotificationMessageAboutNewTasks().c_str());
	}
	const BOOL res = Shell_NotifyIcon(modeAdd ? NIM_ADD : NIM_MODIFY, &nid);
	DestroyIcon(nid.hIcon);
	return !!res;
}

int UI::DeleteNotificationIcon(HWND hWnd) {
	NOTIFYICONDATAW nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(NOTIFYICONDATAW);
	nid.hWnd = hWnd;
	nid.uID = 1;
	return Shell_NotifyIcon(NIM_DELETE, &nid);
}
