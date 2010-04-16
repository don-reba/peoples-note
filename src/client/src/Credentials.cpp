#include "stdafx.h"
#include "Credentials.h"

using namespace std;

Credentials::Credentials()
{
}

Credentials::Credentials
	( const wstring & username
	, const wstring & password
	)
	: username (username)
	, password (password)
{
}

const wstring & Credentials::GetUsername() const
{
	return username;
}

const wstring & Credentials::GetPassword() const
{
	return password;
}

void Credentials::SetUsername(const wstring & username)
{
	this->username = username;
}

void Credentials::SetPassword(const wstring & password)
{
	this->password = password;
}