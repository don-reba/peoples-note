#include "stdafx.h"
#include "MockCredentialsModel.h"

using namespace std;

MockCredentialsModel::MockCredentialsModel
	( wstring username
	, wstring password
	)
	: username (username)
	, password (password)
{
}

ICredentialsModel & MockCredentialsModel::operator =
	( const ICredentialsModel & credentials
	)
{
	username = credentials.GetUsername();
	password = credentials.GetPassword();
	return *this;
}

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
