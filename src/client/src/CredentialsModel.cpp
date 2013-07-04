#include "stdafx.h"
#include "CredentialsModel.h"
#include "Tools.h"

using namespace Tools;

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

	if (StartsWith(this->shard, L"https:"))
		this->shard.replace(0, 6, L"http:");

	SignalSet();
}

void CredentialsModel::Update(const std::wstring & status)
{
	this->status = status;
	SignalUpdate();
}
