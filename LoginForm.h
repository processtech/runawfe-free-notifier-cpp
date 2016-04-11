#include "StdAfx.h"
#include "resource.h"
#include "Logger.h"
#include "Utils.h"
#include "RtnResources.h"
#include "RtnCredentials.h"
using namespace std;
namespace UI{

class LoginForm
{
public:

	LoginForm(Auth::RtnCredentials* credentials);

	~LoginForm(void);

	void Show(HINSTANCE hInstance, HWND parent);

private:
	static HWND CreateEdit(const HWND hParent,const HINSTANCE hInst,DWORD dwStyle, const RECT& rc, const int id, const wstring caption);
	static HWND CreateLabel(const HWND hParent,const HINSTANCE hInst,DWORD dwStyle, const RECT& rc, const int id, const wstring caption);
	static HWND CreateButton(const HWND hParent,const HINSTANCE hInst,DWORD dwStyle, const RECT& rc, const int id, const wstring caption,int hmenu);
	static int OnCreate(const HWND hwnd,CREATESTRUCT *cs);

	static LRESULT CALLBACK LoginForm::PasswordWindowProc(HWND hWnd1, UINT uMsg, WPARAM wParam, LPARAM lParam);
	BOOL LoginForm::RegisterEnterPasswordWindowClass();	
	static HWND loginBox;
	static HWND passwordBox;
	static HWND mainFrom;
	static Auth::RtnCredentials* appCredentials;
};
}

