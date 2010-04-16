#pragma once

class ICredentialsModel;

class ILastUserModel
{
public:

	virtual ~ILastUserModel() {}

	virtual std::wstring GetUsername() = 0;

	virtual void SetUsername(const std::wstring & username) = 0;
};
