#include "stdafx.h"
#include "MockLastUserModel.h"

const ICredentialsModel & MockLastUserModel::GetCredentials()
{
	return credentialsModel;
}
