#pragma once

class ICredentialsModel;

class ILastUserModel
{
public:

	virtual ~ILastUserModel() {}

	virtual const ICredentialsModel & GetCredentials() = 0;
};
