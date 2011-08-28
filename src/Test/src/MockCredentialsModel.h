#pragma once

#include "ICredentialsModel.h"

class MockCredentialsModel : public ICredentialsModel
{
	MacroTestEvent(Commit)
	MacroTestEvent(Set)
	MacroTestEvent(Update)

public:
	
	std::wstring username;
	std::wstring password;
	std::wstring status;

	signal SignalUpdated;
	signal SignalUpdating;

public:

	MockCredentialsModel();

public:

	virtual void Commit();
	
	virtual std::wstring GetUsername() const;

	virtual std::wstring GetPassword() const;

	virtual std::wstring GetStatus() const;

	virtual void Set
		( const std::wstring & username
		, const std::wstring & password
		);

	virtual void Update(const std::wstring & status);
};
