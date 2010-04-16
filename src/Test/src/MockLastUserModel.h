#pragma once

#include "ILastUserModel.h"
#include "MockCredentialsModel.h"

class MockLastUserModel : public ILastUserModel
{
public:

	std::wstring username;

public:

	virtual std::wstring GetUsername();

	virtual void SetUsername(const std::wstring & username);
};
