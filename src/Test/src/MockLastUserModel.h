#pragma once

#include "ILastUserModel.h"
#include "MockCredentialsModel.h"

class MockLastUserModel : public ILastUserModel
{
public:

	MockCredentialsModel credentialsModel;

public:

	virtual const ICredentialsModel & GetCredentials();
};
