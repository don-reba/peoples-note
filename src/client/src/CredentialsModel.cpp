#include "stdafx.h"
#include "CredentialsModel.h"

void CredentialsModel::Commit()
{
	SignalCommit();
}

std::wstring CredentialsModel::GetShard() const
{
	return shard;
}

std::wstring CredentialsModel::GetToken() const
{
	return token;
}

std::wstring CredentialsModel::GetUsername() const
{
	return username;
}

std::wstring CredentialsModel::GetStatus() const
{
	return status;
}

void CredentialsModel::Set
	( const std::wstring & username
	, const std::wstring & token
	, const std::wstring & shard
	)
{
	this->username = username;
	this->token    = token;
	this->shard    = shard;
	SignalSet();
}

void CredentialsModel::Update(const std::wstring & status)
{
	this->status = status;
	SignalUpdate();
}
