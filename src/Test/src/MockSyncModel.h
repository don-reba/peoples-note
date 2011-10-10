#pragma once

#include "ISyncModel.h"

class MockSyncModel : public ISyncModel
{
	MacroTestEvent(SyncComplete)
	MacroTestEvent(NotebooksChanged)
	MacroTestEvent(NotesChanged)
	MacroTestEvent(StatusUpdated)
	MacroTestEvent(TagsChanged)
public:

	bool syncBegan;

	std::wstring documentPath;
	std::wstring username;
	std::wstring password;
	std::wstring statusText;
	double       syncProgress;

public:

	MockSyncModel();

	virtual void BeginSync
		( const std::wstring & username
		, const std::wstring & pasword
		);

	virtual const wchar_t * GetStatusText();

	virtual double GetSyncProgress();
};
