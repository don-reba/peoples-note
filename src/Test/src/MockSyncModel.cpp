#include "stdafx.h"
#include "MockSyncModel.h"

#include "IUserModel.h"

MockSyncModel::MockSyncModel()
	: syncBegan (false)
{
}

void MockSyncModel::BeginSync(IUserModel & userModel)
{
	Credentials credentials;
	userModel.GetCredentials(credentials);

	this->syncBegan    = true;
	this->documentPath = userModel.GetFolder();
	this->username     = credentials.GetUsername();
}

void MockSyncModel::ConnectSyncComplete(slot_type OnSyncComplete)
{
	SignalSyncComplete.connect(OnSyncComplete);
}
