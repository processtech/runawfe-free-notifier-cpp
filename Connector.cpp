#include "StdAfx.h"
#include "Connector.h"
#include "Utils.h"
#include "Logger.h"
#include "RtnResources.h"

Server::State::State(ConnectionState connectionState) {
	this->connectionState = connectionState;
	this->unreadTasksCount = 0;
	this->totalTasksCount = 0;
	this->taskIds = NULL;
	this->taskNames = NULL;
}

Server::State::State(ns1__getMyTasksResponse* response) {
	this->connectionState = CONNECTION_STATE_CONNECTED;
	this->unreadTasksCount = 0;
	this->totalTasksCount = response->__sizeresult;
	this->taskIds = new LONG64[totalTasksCount];
	this->taskNames = new wstring[totalTasksCount];
	for (int i = 0; i < totalTasksCount; i++) {
		if (response->result[i]->firstOpen) {
			this->unreadTasksCount++;
		}
		this->taskIds[i] = *(response->result[i]->id);
		this->taskNames[i] = response->result[i]->name;
	}
}

Server::State::~State(void) {
	delete[] taskIds;
	delete[] taskNames;
}

Server::ConnectionState Server::State::GetConnectionState() {
	return connectionState;
}

int Server::State::GetTotalTasksCount() {
	return totalTasksCount;
}

int Server::State::GetUnreadTasksCount() {
	return unreadTasksCount;
}

bool Server::State::HasChanges(State* previousState) {
	if (connectionState != previousState->connectionState) {
		return true;
	}
	if (totalTasksCount != previousState->totalTasksCount) {
		return true;
	}
	if (unreadTasksCount != previousState->unreadTasksCount) {
		return true;
	}
	for (int i = 0; i < totalTasksCount; i++) {
		bool foundInPrevious = false;
		for (int j = 0; j < previousState->totalTasksCount; j++) {
			if (this->taskIds[i] == previousState->taskIds[j]) {
				foundInPrevious = true;
				break;
			}
		}
		if (!foundInPrevious) {
			return true;
		}
	}
	return false;
}

wstring Server::State::GetNotificationMessageAboutNewTasks(State* previousState) {
	if (connectionState == CONNECTION_STATE_CONNECTED) {
		if (previousState->connectionState != CONNECTION_STATE_CONNECTED && this->unreadTasksCount > 0) {
			return RtnResources::GetOption(L"connection.state.connected.having.unread.tasks", L"You have unread tasks");
		}
		if (this->taskIds != NULL && previousState->taskIds != NULL) {
			wstring message = L"";
			for (int i = 0; i < totalTasksCount; i++) {
				bool foundInPrevious = false;
				for (int j = 0; j < previousState->totalTasksCount; j++) {
					if (this->taskIds[i] == previousState->taskIds[j]) {
						foundInPrevious = true;
						break;
					}
				}
				if (!foundInPrevious) {
					if (!message.empty()) {
						message.append(L"\n");
					}
					message.append(this->taskNames[i]);
				}
			}
			return message;
		}
	}
	return L"";
}

Server::Connector::Connector(Auth::RtnCredentials* appCredentials) {
	this->appCredentials=appCredentials;
	user = NULL;
	previousState = new State(CONNECTION_STATE_INITIALIZING);
	currentState = new State(CONNECTION_STATE_INITIALIZING);	
	initialized = false;
}

Server::Connector::~Connector(void) {
	Reset();
	delete previousState;
	delete currentState;	
}

void Server::Connector::Initialize() {
	if (!initialized) {
		delete currentState;
		this->currentState = new State(CONNECTION_STATE_INITIALIZING);
		serverType = RtnResources::GetServerType();
		serverVersion = RtnResources::GetServerVersion();
		authtype = RtnResources::GetAuthenticationType();
		initialized = true;
	}
}

void Server::Connector::Reset() {
	serverType = L"";
	serverVersion = L"";
	if (user != NULL) {
		delete user;
		user = NULL;
	}
	initialized = false;
}

ns1__user* Server::Connector::AuthenticateByKerberos() {
	LOG_DEBUG("call AuthenticateByKerberos ...");
	const wstring kerberosServerName = RtnResources::GetOption(L"kerberos.spn", L"");
	if (kerberosServerName.empty()) {
		throw "Required property 'kerberos.spn' is not set";
	}
	const string ticket = Auth::GetKerberosTicket(kerberosServerName);
	if(ticket.empty()) {
		LOG_ERROR("GetKerberosTicket failed");
		return false;
	}
	ns1__authenticateByKerberos request;
	const unsigned char* tokenPointer = reinterpret_cast<const unsigned char*>(ticket.c_str());
	request.token = new xsd__base64Binary();
	request.token->__ptr = const_cast<unsigned char*>(tokenPointer);
	request.token->__size = ticket.length();
	ns1__authenticateByKerberosResponse response;
	string authenticationUrl = IO::ToString(RtnResources::GetWebServiceURL(serverType, serverVersion, L"Authentication"));
	ServerAPIBindingProxy authProxy(authenticationUrl.c_str());
	int result = authProxy.authenticateByKerberos(&request, &response);
    if (result == SOAP_OK) {
        LOG_DEBUG("call AuthenticateByKerberos completed");
		return response.result;
    } else {
		LOG_ERROR("call AuthenticateByKerberos failed by '%s'", authenticationUrl.c_str());
		Logger::LogWebServiceError(&authProxy);
		return NULL;
	}
	// TODO authProxy.destroy(); if copy constructor for class ns1__user will be available
}

ns1__user* Server::Connector::AuthenticateByLoginAndPassword() {
	LOG_DEBUG("call AuthenticateByLogin ...");
	ns1__authenticateByLoginPassword request;
	wchar_t loginBuffer[200];
	wchar_t passBuffer[200];
	swprintf(loginBuffer,appCredentials->getLogin().length()+1,L"%s",appCredentials->getLogin().c_str());
	swprintf(passBuffer,appCredentials->getPassword().length()+1,L"%s",appCredentials->getPassword().c_str());
	request.name = loginBuffer;
	request.password = passBuffer;
	string authServiceUrl = IO::ToString(RtnResources::GetWebServiceURL(serverType, serverVersion, L"Authentication"));
	ServerAPIBindingProxy authServiceProxy(authServiceUrl.c_str());
	ns1__authenticateByLoginPasswordResponse response;
	int result = authServiceProxy.authenticateByLoginPassword(&request, &response);
    if (result == SOAP_OK) {
        LOG_DEBUG("call AuthenticateByKerberos completed");
		return response.result;
    } else {
		LOG_ERROR("call AuthenticateByKerberos failed by '%s'", authServiceUrl.c_str());
		Logger::LogWebServiceError(&authServiceProxy);
		return NULL;
	}
}

Server::State* Server::Connector::GetTasks() {
	LOG_DEBUG("call GetMyTasks ...");
	ns1__getMyTasks request;	
	string taskServiceUrl = IO::ToString(RtnResources::GetWebServiceURL(serverType, serverVersion, L"Task"));
	ServerAPIBindingProxy taskServiceProxy(taskServiceUrl.c_str());
	request.user = user;	
	ns1__getMyTasksResponse response;
	int result = taskServiceProxy.getMyTasks(&request, &response);
	State* state;
    if (result == SOAP_OK) {
        LOG_DEBUG("call GetMyTasks completed");
		state = new Server::State(&response);
    } else {
		LOG_ERROR("call GetMyTasks failed by url '%s'", taskServiceUrl.c_str());
		Logger::LogWebServiceError(&taskServiceProxy);
		state = new Server::State(CONNECTION_STATE_ERROR);
	}
	taskServiceProxy.destroy();
	return state;
}

Server::State* Server::Connector::RequestState() {
	State* state;
	try {
		LOG_DEBUG("call RequestState ...");
		Initialize();
		if (user == NULL) {
			if(authtype.compare(BASIC_AUTH)==0){
				this->user = AuthenticateByLoginAndPassword();
			}else{
				this->user = AuthenticateByKerberos();
			}
			if (user == NULL) {
				throw "User is NULL";
			}
		}
		state = GetTasks();
	} catch (const exception& e) {
		LOG_ERROR("exception: %s", e.what());
		state = new Server::State(CONNECTION_STATE_ERROR);
	} catch (const char* e) {
		LOG_ERROR("exception: %s", e);
		state = new Server::State(CONNECTION_STATE_ERROR);
	} catch (const string e) {
		LOG_ERROR("exception: %s", e.c_str());
		state = new Server::State(CONNECTION_STATE_ERROR);
	} catch (const wstring e) {
		LOG_ERROR_W(L"exception: " + e);
		state = new Server::State(CONNECTION_STATE_ERROR);
	} catch(...) {
		LOG_ERROR("General exception catched");
		state = new Server::State(CONNECTION_STATE_ERROR);
	}
	if (state->GetConnectionState() == CONNECTION_STATE_ERROR) {
		Reset();
	}
	return state;
}

void Server::Connector::UpdateState() {
	State* newState = RequestState();
	delete previousState;
	previousState = currentState;
	currentState = newState;
}

bool Server::Connector::HasChanges() {
	return this->currentState->HasChanges(this->previousState);
}

bool Server::Connector::HasNotificationAboutNewTasks() {
	return !this->currentState->GetNotificationMessageAboutNewTasks(this->previousState).empty();
}

wstring Server::Connector::GetNotificationTooltipMessage() {
	wstring message = RtnResources::GetApplicationTitle();
	message.append(L"\n");
	switch(currentState->GetConnectionState()) {
	case CONNECTION_STATE_INITIALIZING:
		message.append(RtnResources::GetOption(L"connection.state.initializing", L"Initializing application"));
		break;
	case CONNECTION_STATE_CONNECTED: {
		message.append(RtnResources::GetOption(L"connection.state.connected.total.tasks", L"Total tasks"));
		message.append(L": ");
		wchar_t buf[2 * _MAX_INT_DIG];
		std::swprintf(buf, L"%d", currentState->GetTotalTasksCount());
		message.append(wstring(buf));
		message.append(L"\n");
		
		message.append(RtnResources::GetOption(L"connection.state.connected.unread.tasks", L"Unread tasks"));
		message.append(L": ");
		wchar_t buf1[2 * _MAX_INT_DIG];
		std::swprintf(buf1, L"%d", currentState->GetUnreadTasksCount());
		message.append(wstring(buf1));
		break;
	}
	case CONNECTION_STATE_ERROR:
		message.append(RtnResources::GetOption(L"connection.state.error", L"Error"));
		break;
	}
	return message;
}

HICON Server::Connector::GetNotificationIcon() {
	wstring fileName;
	switch(currentState->GetConnectionState()) {
	case CONNECTION_STATE_INITIALIZING:
		fileName = L"rtn.initializing.ico";
		break;
	case CONNECTION_STATE_CONNECTED: {
		if (currentState->GetUnreadTasksCount() == 0) {
			fileName = L"rtn.app.ico";
		} else {
			fileName = L"rtn.connected.has.new.tasks.ico";
		}
		break;
	 }
	case CONNECTION_STATE_ERROR:
		fileName = L"rtn.error.ico";
		break;
	}
	wstring filePath = IO::GetFilePath(fileName);
	HICON icon = (HICON) LoadImage(NULL, filePath.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
	if (!icon) {
		LOG_ERROR_W(L"Unable to load icon '" + fileName + L"'");
	}
	return icon;
}

wstring Server::Connector::GetNotificationMessageAboutNewTasks() {
	return this->currentState->GetNotificationMessageAboutNewTasks(this->previousState);
}
