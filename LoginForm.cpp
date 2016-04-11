#include "StdAfx.h"
#include "LoginForm.h"
#include "resource.h"

#include "Logger.h"
#include "Utils.h"
#include "RtnResources.h"
#include "Connector.h"
#include "WinUser.h"
#define ENTER_PASSWORD_CLASS_NAME L"RtnEnterPasswordWndClass"
using namespace std;

enum {
  IDCE_SINGLELINE = 200,  
};


HWND UI::LoginForm::loginBox;
HWND UI::LoginForm:: passwordBox;
HWND UI::LoginForm::mainFrom;
Auth::RtnCredentials* UI::LoginForm::appCredentials;

UI::LoginForm::LoginForm(Auth::RtnCredentials* credentials ){		
	RegisterEnterPasswordWindowClass();		
	appCredentials=credentials;
}

UI::LoginForm::~LoginForm(void){
}

void UI::LoginForm::Show(HINSTANCE hInstance, HWND parent){
	RECT rc;
	HWND hWndEnterWindow = CreateWindowW(ENTER_PASSWORD_CLASS_NAME, RtnResources::GetLabelLoginTitle().c_str(), WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME | WS_EX_DLGMODALFRAME | WS_EX_TOPMOST, 100, 100, 200, 200, NULL, NULL, hInstance, NULL);	
	GetWindowRect ( hWndEnterWindow, &rc );
	int xPos = (GetSystemMetrics(SM_CXSCREEN) - rc.right)/2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - rc.bottom)/2;
	SetWindowPos( hWndEnterWindow, 0,  xPos, yPos, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
	mainFrom = parent;
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

		DWORD dwStyle = 0;
	
	if (hWndEnterWindow != NULL){
		BOOL bRet = FALSE;
	    ShowWindow(hWndEnterWindow, SW_SHOWNORMAL);
		UpdateWindow(hWndEnterWindow);
		while (GetMessageW(&msg, NULL, 0, 0)>0) {	
			if (!IsDialogMessage(hWndEnterWindow, &msg)) {
					TranslateMessage(&msg);
					DispatchMessage(&msg);
			}				
		}		
	}
}


LRESULT CALLBACK UI::LoginForm::PasswordWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_CREATE: 		
		return OnCreate(hWnd,reinterpret_cast<CREATESTRUCT*>(lParam));		
		break;			
	case WM_CLOSE:
		  SendMessage(mainFrom, WM_COMMAND, ID__QUIT,0 );
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:		
            PostQuitMessage(0);
		break;
	case WM_COMMAND:
        if(LOWORD(wParam)==1000){
           SendMessage(mainFrom, WM_COMMAND, ID__QUIT,0 );
		  DestroyWindow(hWnd);
        }
		if(LOWORD(wParam)==1001){
			DWORD loginTextLength = GetWindowTextLength(UI::LoginForm::loginBox)+1;			
			LPWSTR pszText = (LPWSTR)GlobalAlloc(GPTR, loginTextLength*sizeof(LPWSTR));			
			if(pszText != NULL)
			{
				wstring toSetLogin;
				GetWindowText(UI::LoginForm::loginBox, pszText, loginTextLength);				
				toSetLogin=pszText;
				appCredentials->setLogin(toSetLogin);
			}
			GlobalFree(pszText);
			DWORD poasswordTextLength = GetWindowTextLength(UI::LoginForm::passwordBox)+1;
			pszText = (LPWSTR)GlobalAlloc(GPTR, poasswordTextLength*sizeof(LPWSTR));			
			if(pszText != NULL)
			{
				wstring toSetPassword;
				GetWindowText(UI::LoginForm::passwordBox, pszText, poasswordTextLength);				
				toSetPassword=pszText;
				appCredentials->setPassword(toSetPassword);
			}			
			GlobalFree(pszText);
			SendMessage(mainFrom, WM_COMMAND, ID_LOGIN_READY, 0);
			DestroyWindow(hWnd);
		}
        break;
	default:		
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

int UI::LoginForm::OnCreate(const HWND hwnd,CREATESTRUCT *cs)
{	
	RECT rc = {10,10,150,28};
	CreateLabel(hwnd,cs->hInstance,0,rc,IDCE_SINGLELINE,RtnResources::GetLabelLoginText());
	rc.top+=20;
	UI::LoginForm::loginBox = CreateEdit(hwnd,cs->hInstance,0,rc,IDCE_SINGLELINE,appCredentials->getLogin());	
	SetFocus(UI::LoginForm::loginBox);
	rc.top+=40;	
	CreateLabel(hwnd,cs->hInstance,0,rc,IDCE_SINGLELINE,RtnResources::GetLabelPasswordText());
	rc.top+=20;
	UI::LoginForm::passwordBox = CreateEdit(hwnd,cs->hInstance,ES_PASSWORD,rc,IDCE_SINGLELINE, appCredentials->getPassword());
	rc.top+=40;
	rc.right-=73;
	CreateButton(hwnd,cs->hInstance,ES_PASSWORD,rc,IDCE_SINGLELINE, RtnResources::GetButtonLoginText(),1001);
	rc.left+=81;
	CreateButton(hwnd,cs->hInstance,ES_PASSWORD,rc,IDCE_SINGLELINE,  RtnResources::GetOption(L"menu.exit", L"Quit"),1000);	
	return 0;
}


BOOL UI::LoginForm::RegisterEnterPasswordWindowClass(){
	WNDCLASSEXW wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON_APP));
	wc.hIconSm = LoadIcon(NULL, MAKEINTRESOURCE(IDI_ICON_APP));	
	wc.lpfnWndProc = &UI::LoginForm::PasswordWindowProc;
	wc.lpszClassName = ENTER_PASSWORD_CLASS_NAME;	
	wc.lpszMenuName = NULL;
	wc.style = 0;
	if (!RegisterClassEx(&wc)) {
		return false;
	}
	return true;
}

HWND UI::LoginForm::CreateEdit(const HWND hParent,const HINSTANCE hInst,DWORD dwStyle,
                const RECT& rc,const int id,const wstring caption)
{
dwStyle|=WS_CHILD | WS_VISIBLE | WS_TABSTOP;
return CreateWindowEx(WS_EX_CLIENTEDGE,             //extended styles
                      _T("edit"),                   //control 'class' name
                      caption.c_str(),              //control caption
                      dwStyle,                      //control style 
                      rc.left,                      //position: left
                      rc.top,                       //position: top
                      rc.right,                     //width
                      rc.bottom,                    //height
                      hParent,                      //parent window handle
                      //control's ID
                      reinterpret_cast<HMENU>(static_cast<INT_PTR>(id)),
                      hInst,                        //application instance
                      0);                           //user defined info
}

HWND UI::LoginForm::CreateLabel(const HWND hParent, const HINSTANCE hInst, DWORD dwStyle, const RECT& rc, const int id, const wstring text){

	HWND hwnd_ed_u = CreateWindow(_T("static"), _T(""),
                              WS_CHILD | WS_VISIBLE | ES_LEFT,
                              rc.left, rc.top, rc.right, rc.bottom,
                              hParent, (HMENU)(501),
                             hInst, NULL);
	SetWindowTextW(hwnd_ed_u, text.c_str());
    return hwnd_ed_u;
}

HWND UI::LoginForm::CreateButton(const HWND hParent, const HINSTANCE hInst, DWORD dwStyle, const RECT& rc, const int id, const wstring text, int hmenu){

	HWND hwnd_ed_u = CreateWindow(_T("button"), text.c_str(),
                              WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON| WS_TABSTOP,
                              rc.left, rc.top, rc.right, rc.bottom,
                              hParent, (HMENU)(hmenu),
                             hInst, NULL);	
    return hwnd_ed_u;
}
