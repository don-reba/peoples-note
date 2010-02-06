#include "stdafx.h"
#include "MockUserModel.h"

MockUserModel::MockUserModel()
	: isDefault (false)
	, isLoaded  (false)
{
}

void MockUserModel::Loaded()
{
	SignalLoaded();
}

void MockUserModel::ConnectLoaded(slot_type OnLoaded)
{
	SignalLoaded.connect(OnLoaded);
}

void MockUserModel::CreateDefaultUser()
{
	isDefault = true;
}

void MockUserModel::SetCredentials(const ICredentialsModel & credentials)
{
	credentialsModel = credentials;
}

INotebook & MockUserModel::GetLastUsedNotebook()
{
	return lastUsedNotebook;
}

void MockUserModel::Load()
{
	isLoaded = true;
}
