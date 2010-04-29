#pragma once

#include "Guid.h"

#include <map>

class SyncLogic
{
private:

	enum Action
	{
		ActionAdd,
		ActionDelete,
		ActionMerge,
		ActionNone,
		ActionRename,
		ActionUpload,
	};

public:

	template<typename T>
	void FullSync
		( std::vector<T> & remote
		, std::vector<T> & local
		, void (*Add)(T&)
		, void (*Delete)(T&)
		, void (*Rename)(T&)
		, void (*Upload)(T&)
		, void (*Merge)(T&, T&)
		);

private:

	Action DecideFullSyncRemote
		( const Guid & remoteGuid
		, int          remoteUsn
		);

	Action DecideFullSyncLocal
		( const Guid & localGuid
		, int          localUsn
		, bool         localDirty
		);

	Action DecideFullSyncConflict
		( const std::wstring & remoteGuid
		, int                  remoteUsn
		, const std::wstring & localGuid
		, int                  localUsn
		, bool                 localDirty
		);

	Action DecideIncrementalSyncRemote
		( const Guid & remoteGuid
		);

	Action DecideIncrementalSyncLocal
		( const Guid & localGuid
		, bool         localDirty
		);

	Action DecideIncrementalSyncConflict
		( const Guid & remoteGuid
		, const Guid & localGuid
		, bool         localDirty
		);
};

template<typename T>
void SyncLogic::FullSync
	( std::vector<T> & remote
	, std::vector<T> & local
	, void (*Add)(T&)
	, void (*Delete)(T&)
	, void (*Rename)(T&)
	, void (*Upload)(T&)
	, void (*Merge)(T&, T&)
	)
{
	typedef std::map<std::string, T*> Map;

	Map localGuids;
	foreach (T & l, local)
		localGuids[l.GetGuid()] = &l;

	Map remoteGuids;
	foreach (T & r, remote)
		remoteGuids[r.GetGuid()] = &r;

	foreach (T & r, remote)
	{
		Map::iterator l(localGuids.find(r.GetGuid()));
		Action action
			= (l == localGuids.end())
			? DecideFullSyncRemote
				( r.GetGuid()
				, r.GetUsn()
				)
			: DecideFullSyncConflict
				( r.GetName()
				, r.GetUsn()
				, l->second->GetName()
				, l->second->GetUsn()
				, l->second->IsDirty()
				)
			;
		switch (action)
		{
		case ActionAdd:    Add(r);               break;
		case ActionDelete: Delete(*l->second);   break;
		case ActionRename: Rename(*l->second);   break;
		case ActionUpload: Upload(*l->second);   break;
		case ActionMerge:  Merge(*l->second, r); break;
		}
	}

	foreach (T & l, local)
	{
		Map::iterator r(remoteGuids.find(l.GetGuid()));
		Action action
			= (r == remoteGuids.end())
			? DecideFullSyncLocal
				( l.GetGuid()
				, l.GetUsn()
				, l.IsDirty()
				)
			: ActionNone
			;
		switch (action)
		{
		case ActionAdd:    Add(*r->second);      break;
		case ActionDelete: Delete(l);            break;
		case ActionRename: Rename(l);            break;
		case ActionUpload: Upload(l);            break;
		case ActionMerge:  Merge(l, *r->second); break;
		}
	}
}
