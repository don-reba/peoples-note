#include "stdafx.h"
#include "MockCredentialsModel.h"

using namespace std;

MockCredentialsModel::MockCredentialsModel()
{
}

MockCredentialsModel::MockCredentialsModel
	( wstring username
	, wstring password
	)
	: username (username)
	, password (password)
{
}

void  MockCredentialsModel::ConnectNeedsSet(slot_type OnNeedsSet)
{
	SignalNeedsSet.connect(OnNeedsSet);
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
