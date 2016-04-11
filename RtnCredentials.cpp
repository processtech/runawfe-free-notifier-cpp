#include "StdAfx.h"
#include "RtnCredentials.h"


Auth::RtnCredentials::RtnCredentials(void){	
}

Auth::RtnCredentials::~RtnCredentials(void){
}

wstring Auth::RtnCredentials::getPassword(){
	return this->password;
}

wstring Auth::RtnCredentials::getLogin(){
	return this->login;
}

void Auth::RtnCredentials::setPassword(wstring password){
	this->password = password;
}

void Auth::RtnCredentials::setLogin(wstring login){
	this->login = login;
}
