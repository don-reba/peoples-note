#pragma once

class ICredentialsModel
{
public:

	virtual ~ICredentialsModel() {}

	virtual ICredentialsModel & operator =
		( const ICredentialsModel & credentials
		) = 0;
	
	virtual std::wstring GetUsername() const = 0;

	virtual std::wstring GetPassword() const = 0;

	virtual void SetCredentials
		( std::wstring username
		, std::wstring password
		) = 0;
};
