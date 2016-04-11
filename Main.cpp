//------------------------------------------------------
#include "stdafx.h"
#include "resource.h"

#include "Logger.h"
#include "Utils.h"
#include "RtnResources.h"
#include "Connector.h"
#include "LoginForm.h"
using namespace std;

//------------------------------------------------------

#undef NOTIFYICONDATAW_V2_SIZE
#define NOTIFYICONDATAW_V2_SIZE RTL_SIZEOF_THROUGH_FIELD(NOTIFYICONDATAW, dwInfoFlags)

//------------------------------------------------------

#define WINDOW_CLASS_NAME		L"RtnLauncherWndClass"
#define PROGRAM_MUTEX_NAME		L"RtnLauncherMutex"


//------------------------------------------------------

HINSTANCE				hInstance = NULL;
HWND mainHWnd;
UI::LoginForm loginForm = NULL;
Auth::RtnCredentials credentials = Auth::RtnCredentials();
// Following variable is used to handle case when explorer.exe is restarted.
// In this case our app will create systray icon again.
// To get more info about TaskbarCreated window message read here:
// http://msdn.microsoft.com/en-us/library/windows/desktop/cc144179%28v=vs.85%29.aspx
UINT					taskbarCreatedMessageId = 0;
Server::Connector* connector;

/**
* if this flag false, programm will ask user to enter login and password;
* if true = programm wil sync with server;
*/
bool startSync = false;

void SynchronizeWithServer(HWND hWnd) {
	if(startSync){
		connector->UpdateState();
		UI::UpdateNotificationIcon(hWnd, connector);
	}
}

void OnTimer(HWND hWnd) {
	SynchronizeWithServer(hWnd);
}


void ShowEnterPasswordWindow(HWND hWnd){	
	loginForm.Show(hInstance, hWnd);
}

void ShowContextMenu(HWND hWnd, POINT pt) {
	HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU_CONTEXT));
	if (hMenu) {
		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		if (hSubMenu) {
			// our window must be foreground before calling TrackPopupMenu or the menu will not disappear when the user clicks away
			SetForegroundWindow(hWnd);
			UI::SetMenuLabel(hMenu, ID__BROWSE, RtnResources::GetOption(L"menu.open", L"Open"));
			UI::SetMenuLabel(hMenu, ID__SETTINGS, RtnResources::GetOption(L"menu.update", L"Update status"));
			UI::SetMenuLabel(hMenu, ID__QUIT, RtnResources::GetOption(L"menu.exit", L"Quit"));
			// respect menu drop alignment
			UINT uFlags = TPM_RIGHTBUTTON;
			if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0) {
				uFlags |= TPM_RIGHTALIGN;
			} else {
				uFlags |= TPM_LEFTALIGN;
			}
			TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hWnd, NULL);
		} else {
			LOG_ERROR("Error loading submenu");
		}
		DestroyMenu(hMenu);
	} else {
		LOG_ERROR("Error loading menu");
	}
}

void StartBrowse(HWND hwnd){
	if(RtnResources::GetAuthenticationType().compare(BASIC_AUTH)==0){
				UI::LaunchBrowser(hwnd,credentials.getLogin(),credentials.getPassword());
			}else{
				UI::LaunchBrowser(hwnd);
			}
}

LRESULT CALLBACK WindowProcMain(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: {
			taskbarCreatedMessageId = RegisterWindowMessage(L"TaskbarCreated"); 
			connector = new Server::Connector(&credentials);
			//Set connector properties
			if (!UI::UpdateNotificationIcon(hWnd, connector, true)) {
				UI::QuitApplication(hWnd, connector, "systray not initialized");
				return -1;
			}
			startSync = RtnResources::GetUserInputLoginSilenty();
			if(!startSync){
				//Show enter login window
				ShowEnterPasswordWindow(hWnd);
			}
			SynchronizeWithServer(hWnd);
			const int timeoutInSeconds = RtnResources::GetOptionInt(L"check.tasks.timeout", 60);
			LOG_DEBUG("Starting timer with timeout %d sec", timeoutInSeconds);
			SetTimer(hWnd, 1, timeoutInSeconds * 1000, NULL);
		}
		break;
	case WM_TIMER:
		OnTimer(hWnd);
		break;
	case WM_CUSTOM_NOTIFY_ICON:
		switch (LOWORD(lParam)) {
		case NIN_BALLOONUSERCLICK:
		case WM_LBUTTONDBLCLK:
			StartBrowse(hWnd);
			break;
		case WM_RBUTTONUP:
			POINT pt;
			GetCursorPos(&pt);
			ShowContextMenu(hWnd, pt);
			break;
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID__QUIT:
			UI::QuitApplication(hWnd, connector, "menu event");
			break;
		case ID__SETTINGS:
			SynchronizeWithServer(hWnd);
			break;
		case ID__BROWSE:
			StartBrowse(hWnd);
			break;
		case ID_LOGIN_READY:
			startSync=true;
			SynchronizeWithServer(hWnd);
			break;
		}
		break;
	case WM_DESTROY:
		if (!UI::DeleteNotificationIcon(hWnd)) {
			LOG_ERROR("Error deleting notification icon");
			return -1;
		}
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		if(uMsg == taskbarCreatedMessageId) {
			if (!UI::UpdateNotificationIcon(hWnd, connector, true)) {
				UI::QuitApplication(hWnd, connector, "systray not initialized [2]");
				return -1;
			}
		}
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}



BOOL RegisterMainWindowClass() {
	WNDCLASSEXW wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON_APP));
	wc.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON_APP));
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProcMain;
	wc.lpszClassName = WINDOW_CLASS_NAME;
	wc.lpszMenuName = NULL;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	if (!RegisterClassEx(&wc)) {
		return false;
	}
	return true;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	_CrtSetBreakAlloc(1);
	hInstance = hInstance;
	RtnResources::Init(L"application.properties");
	credentials.setLogin(RtnResources::GetUserInputDefaultLogin());
	credentials.setPassword(RtnResources::GetUserInputDefaultPassword());	
	loginForm = UI::LoginForm(&credentials);
	Logger::Init();
	//SetAssertMessageBoxHandler(OnAssertMessageBox, NULL);
	LOG_INFO("===============================================================================\n");
	// Check if this is a first copy of program in current Windows Terminal session:
	SetLastError(0);
	HANDLE hSingleObject = ::CreateMutex(NULL, FALSE, L"Local\\" PROGRAM_MUTEX_NAME);
	if (hSingleObject && GetLastError() == ERROR_ALREADY_EXISTS) {
		LOG_WARN("Process is already running!\n");
		return 1;
	}
	if (!hSingleObject) {
		LOG_ERROR("Can't create mutex! Error = %d\n", GetLastError());
		return 1;
	}
	//HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE); // for ShellExecute()
	//if (FAILED(hr))	{
	//	LOG_ERROR("Failed to initialize COM library. Error code = 0x%08X", hr);
	//	return 1;
	//}

	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	if (!RegisterMainWindowClass()) {
		return 1;
	}
	DWORD dwStyle = 0;
	mainHWnd = CreateWindowW(WINDOW_CLASS_NAME, L"RunaWFE tasks notifier", dwStyle, 100, 100, 200, 200, NULL, NULL, hInstance, NULL);
	if (mainHWnd != NULL) {
		BOOL bRet = FALSE;
		while ((bRet = GetMessageW(&msg, NULL, 0, 0))) {
			if (bRet == -1) {
				// handle the error and possibly exit
			} else {
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}
	} else {
		DWORD error = GetLastError();
		int i = 0;
	}


	//CoUninitialize();
	CloseHandle(hSingleObject);
	return 0;
}



