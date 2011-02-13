#pragma once

#include "ISignalProvider.h"

class IUserModel;

class ISyncModel : public ISignalProvider
{
	MacroIEvent(NotebooksChanged)
	MacroIEvent(NotesChanged)
	MacroIEvent(StatusUpdated)
	MacroIEvent(TagsChanged)
	MacroIEvent(SyncComplete)

public:

	virtual void BeginSync(const std::wstring & username) = 0;

	virtual const wchar_t * GetStatusText() = 0;

	virtual double GetSyncProgress() = 0;
};
