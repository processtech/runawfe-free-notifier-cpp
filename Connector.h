#pragma once
#include "ws/ServerAPIBindingProxy.h"
#include "RtnResources.h"
#include "RtnCredentials.h"
using namespace std;
namespace Server {

enum ConnectionState {
	CONNECTION_STATE_INITIALIZING,
	CONNECTION_STATE_CONNECTED,
	CONNECTION_STATE_ERROR
};

class State {
private:
	ConnectionState connectionState;
	int totalTasksCount;
	int unreadTasksCount;
	LONG64* taskIds;
	wstring* taskNames;
public:
	State(ConnectionState connectionState);
	State(ns1__getMyTasksResponse* response);
	~State();
	ConnectionState GetConnectionState();
	int GetTotalTasksCount();
	int GetUnreadTasksCount();
	bool HasChanges(State* previousState);
	wstring GetNotificationMessageAboutNewTasks(State* previousState);
};

class Connector {
public:
	Connector(Auth::RtnCredentials* appCredentials);
	~Connector();
	void Initialize();
	void Reset();
	void UpdateState();
	bool HasChanges();
	bool HasNotificationAboutNewTasks();
	wstring GetNotificationTooltipMessage();
	HICON GetNotificationIcon();
	wstring GetNotificationMessageAboutNewTasks();
private:
	Auth::RtnCredentials* appCredentials;
	bool initialized;
	wstring serverType;
	wstring serverVersion;
	State* previousState;
	State* currentState;
	ns1__user* user;
	ns1__user* AuthenticateByKerberos();
	ns1__user* AuthenticateByLoginAndPassword();
	State* GetTasks();
	State* Connector::RequestState();
	char login[200];
	char password[200];
	wstring authtype;
};

}