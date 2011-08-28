#pragma once
#include "ILastUserModel.h"

#include "MockCredentialsModel.h"

class MockLastUserModel : public ILastUserModel
{
public:

	std::wstring password;
	std::wstring username;

public:

	virtual std::wstring GetPassword() const;

	virtual std::wstring GetUsername() const;

	virtual void SetCredentials
		( const std::wstring & username
		, const std::wstring & password
		);
};
