#include "stdafx.h"
#include "MockCredentialsModel.h"

std::wstring MockCredentialsModel::GetUsername() const
{
	return username;
}

std::wstring MockCredentialsModel::GetPassword() const
{
	return password;
}

void MockCredentialsModel::SetCredentials
	( std::wstring username
	, std::wstring password
	)
{
	this->username = username;
	this->password = password;
}
