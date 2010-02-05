#include "stdafx.h"
#include "MockUserModel.h"

MockUserModel::MockUserModel()
	: isDefault (false)
	, isLoaded  (false)
{
}

void MockUserModel::CreateDefaultUser()
{
	isDefault = true;
}

void MockUserModel::SetCredentials(const ICredentialsModel & credentials)
{
	credentialsModel = credentials;
}

void MockUserModel::Load()
{
	isLoaded = true;
}
