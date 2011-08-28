#pragma once

class ILastUserModel
{
public:

	virtual ~ILastUserModel() {}

	virtual std::wstring GetPassword() const = 0;

	virtual std::wstring GetUsername() const = 0;

	virtual void SetCredentials
		( const std::wstring & username
		, const std::wstring & password
		) = 0;
};
