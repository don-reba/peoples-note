#pragma once

class ILastUserModel
{
public:

	virtual ~ILastUserModel() {}

	virtual std::wstring GetToken() const = 0;

	virtual std::wstring GetShard() const = 0;

	virtual std::wstring GetUsername() const = 0;

	virtual void SetCredentials
		( const std::wstring & username
		, const std::wstring & token
		, const std::wstring & shard
		) = 0;
};
