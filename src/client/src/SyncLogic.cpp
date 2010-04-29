#include "stdafx.h"
#include "SyncLogic.h"

#include "Guid.h"

using namespace std;

SyncLogic::Action SyncLogic::DecideFullSyncRemote
	( const Guid & remoteGuid
	, int          remoteUsn
	)
{
	return ActionAdd;
}

SyncLogic::Action SyncLogic::DecideFullSyncLocal
	( const Guid & localGuid
	, int          localUsn
	, bool         localDirty
	)
{
	if (localDirty)
		return ActionUpload;
	else
		return ActionDelete;
}

SyncLogic::Action SyncLogic::DecideFullSyncConflict
	( const wstring & remoteName
	, int             remoteUsn
	, const wstring & localName
	, int             localUsn
	, bool            localDirty
	)
{
	if (localName == remoteName)
	{
		if (localDirty)
			return ActionMerge;
		else
			return ActionRename;
	}
	else
	{
		if (localUsn == remoteUsn)
		{
			if (localDirty)
				return ActionUpload;
			else
				return ActionNone;
		}
		else
		{
			if (localDirty)
				return ActionMerge;
			else
				return ActionAdd;
		}
	}
}

SyncLogic::Action SyncLogic::DecideIncrementalSyncRemote
	( const Guid & remoteGuid
	)
{
	return ActionAdd;
}

SyncLogic::Action SyncLogic::DecideIncrementalSyncLocal
	( const Guid & localGuid
	, bool         localDirty
	)
{
	if (localDirty)
		return ActionUpload;
	else
		return ActionDelete;
}

SyncLogic::Action SyncLogic::DecideIncrementalSyncConflict
	( const Guid & remoteGuid
	, const Guid & localGuid
	, bool         localDirty
	)
{
	if (localGuid == remoteGuid)
	{
		if (localDirty)
			return ActionMerge;
		else
			return ActionAdd;
	}
	else
	{
		if (localDirty)
			return ActionMerge;
		else
			return ActionRename;
	}
}
