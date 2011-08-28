#include "stdafx.h"
#include "MockCredentialsModel.h"

using namespace std;

MockCredentialsModel::MockCredentialsModel()
{
}

void MockCredentialsModel::Commit()
{
	SignalCommit();
}

wstring MockCredentialsModel::GetUsername() const
{
	return username;
}

wstring MockCredentialsModel::GetPassword() const
{
	return password;
}

wstring MockCredentialsModel::GetStatus() const
{
	return status;
}

void MockCredentialsModel::Set
	( const wstring & username
	, const wstring & password
	)
{
	this->username = username;
	this->password = password;
	SignalSet();
}

void MockCredentialsModel::Update(const wstring & status)
{
	this->status = status;
	SignalUpdate();
}
