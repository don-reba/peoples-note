#include "stdafx.h"
#include "MockLastUserModel.h"

using namespace std;

wstring MockLastUserModel::GetPassword() const
{
	return password;
}

wstring MockLastUserModel::GetUsername() const
{
	return username;
}

void MockLastUserModel::SetCredentials
	( const wstring & username
	, const wstring & password
	)
{
	this->username = username;
	this->password = password;
}
