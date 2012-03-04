#pragma once
#include "ISignalProvider.h"

#include "Guid.h"

class IUserModel;

class ISyncModel : public ISignalProvider
{
	MacroIEvent(StatusUpdated)
	MacroIEvent(SyncComplete)

public:

	virtual void BeginSync
		( const std::wstring & username
		, const std::wstring & password
		, const Guid         & notebook
		) = 0;

	virtual const wchar_t * GetStatusText() = 0;

	virtual double GetSyncProgress() = 0;
};
