#pragma once

#include "ISyncModel.h"

class MockSyncModel : public ISyncModel
{
	MacroTestEvent(SyncComplete)
	MacroTestEvent(StatusUpdated)
public:

	bool syncBegan;

	std::wstring documentPath;
	std::wstring username;
	std::wstring password;
	std::wstring statusText;
	Guid         notebook;
	double       syncProgress;

public:

	MockSyncModel();

	virtual void BeginSync
		( const std::wstring & username
		, const std::wstring & pasword
		, const Guid         & notebook
		);

	virtual const wchar_t * GetStatusText();

	virtual double GetSyncProgress();
};
