#pragma once

#include "ICredentialsModel.h"

class ILastUserModel
{
public:

	virtual const ICredentialsModel & GetCredentials() = 0;
};
