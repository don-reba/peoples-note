#include "stdafx.h"
#include "CredentialsModel.h"

ICredentialsModel & CredentialsModel::operator =
	( const ICredentialsModel & credentials
	)
{
	username = credentials.GetUsername();
	password = credentials.GetPassword();
	return *this;
}

std::wstring CredentialsModel::GetUsername() const
{
	return username;
}

std::wstring CredentialsModel::GetPassword() const
{
	return password;
}

void CredentialsModel::SetCredentials
	( std::wstring username
	, std::wstring password
	)
{
	this->username = username;
	this->password = password;
}
