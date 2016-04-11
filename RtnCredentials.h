#pragma once
using namespace std;

namespace Auth{

class RtnCredentials
{
public:
	RtnCredentials(void);
	~RtnCredentials(void);
	wstring getPassword();
	wstring getLogin();
	void setPassword(wstring password);
	void setLogin(wstring login);
private:
	wstring login;
	wstring password;
};
}

