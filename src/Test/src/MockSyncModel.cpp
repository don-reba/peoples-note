#include "stdafx.h"
#include "MockSyncModel.h"

#include "IUserModel.h"

using namespace std;

MockSyncModel::MockSyncModel()
	: syncBegan (false)
{
}

void MockSyncModel::BeginSync
	( const wstring & username
	, const wstring & password
	, const Guid    & notebook
	)
{
	this->syncBegan = true;
	this->username  = username;
	this->password  = password;
	this->notebook  = notebook;
}

const wchar_t * MockSyncModel::GetStatusText()
{
	return statusText.c_str();
}

double MockSyncModel::GetSyncProgress()
{
	return syncProgress;
}
