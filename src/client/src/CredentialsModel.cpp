#include "stdafx.h"
#include "CredentialsModel.h"


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
	SignalUpdated();
}

void CredentialsModel::Update()
{
	SignalUpdating();
}
