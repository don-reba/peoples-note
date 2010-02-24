#pragma once

class ICredentialsModel;

class ILastUserModel
{
public:

	virtual const ICredentialsModel & GetCredentials() = 0;
};
