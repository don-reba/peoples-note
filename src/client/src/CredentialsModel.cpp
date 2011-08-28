#include "stdafx.h"
#include "CredentialsModel.h"

void CredentialsModel::Commit()
{
	SignalCommit();
}

std::wstring CredentialsModel::GetUsername() const
{
	return username;
}

std::wstring CredentialsModel::GetPassword() const
{
	return password;
}

std::wstring CredentialsModel::GetStatus() const
{
	return status;
}

void CredentialsModel::Set
	( const std::wstring & username
	, const std::wstring & password
	)
{
	this->username = username;
	this->password = password;
	SignalSet();
}

void CredentialsModel::Update(const std::wstring & status)
{
	this->status = status;
	SignalUpdate();
}
