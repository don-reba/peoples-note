#pragma once

class Credentials
{
private:

	std::wstring username;
	std::wstring password;

public:

	Credentials();

	Credentials
		( const std::wstring & username
		, const std::wstring & password
		);

	const std::wstring & GetUsername() const;
	const std::wstring & GetPassword() const;

	void SetUsername(const std::wstring & username);
	void SetPassword(const std::wstring & password);
};
