// Implements the Evernote synchronization spec.
// The difference between full sync logic and incremental is that the former
// deletes clean strictly local notes.

#pragma once

#include "Guid.h"

#include <map>

//------
// types
//------

enum ActionType
{
	SyncActionAdd,    // add note from the server
	SyncActionDelete, // delete note on the server
	SyncActionUpload, // add note to the server
	SyncActionMerge,  // merge two conflicting notes

	// not implemented
	// ActionRenameAdd, // rename and add note from the server
};

template<typename T>
struct SyncAction
{
	ActionType Type;
	const T  * Local;
	const T  * Remote;

	SyncAction
		( ActionType type
		, const T * local
		, const T * remote
		)
		: Type   (type)
		, Local  (local)
		, Remote (remote)
	{
	}
};

//----------
// functions
//----------

template<typename T>
void ComputeFullSyncActions
	( const std::vector<T>    & remote
	, const std::vector<T>    & local
	, std::vector<SyncAction<T> > & actions
	)
{
	typedef std::map<std::string,  const T*> GuidMap;
	typedef std::map<std::wstring, const T*> NameMap;

	GuidMap localGuids;
	NameMap localNames;
	foreach (const T & l, local)
	{
		localGuids[l.guid] = &l;
		localNames[l.name] = &l;
	}

	GuidMap remoteGuids;
	NameMap remoteNames;
	foreach (const T & r, remote)
	{
		remoteGuids[r.guid] = &r;
		remoteNames[r.name] = &r;
	}

	foreach (const T & r, remote)
	{
		GuidMap::iterator l(localGuids.find(r.guid));
		if (l == localGuids.end())
		{
			NameMap::iterator l(localNames.find(r.name));
			if (l == localNames.end())
			{
				actions.push_back(SyncAction<T>(SyncActionAdd, NULL, &r));
			}
			else
			{
				const T & l(*l->second);
				if (l.isDirty)
				{
					actions.push_back(SyncAction<T>(SyncActionMerge, &l, &r));
				}
				else
				{
					actions.push_back(SyncAction<T>(SyncActionDelete, &l,   NULL));
					actions.push_back(SyncAction<T>(SyncActionAdd,    NULL, &r));
					// The official document advises renaming,
					// but that does not work well for notes.
					// Replacement seems to be a more sensible
					// approach in general.
					// actions.push_back(SyncAction<T>(ActionRenameAdd, &l, &r));
				}
			}
		}
		else
		{
			const T & l(*l->second);
			if (l.isDirty)
			{
				if (l.usn != r.usn)
					actions.push_back(SyncAction<T>(SyncActionMerge, &l, &r));
				else
					actions.push_back(SyncAction<T>(SyncActionUpload, &l, NULL));
			}
			else
			{
				if (l.usn != r.usn)
					actions.push_back(SyncAction<T>(SyncActionAdd, NULL, &r));
			}
		}
	}

	foreach (const T & l, local)
	{
		GuidMap::iterator r(remoteGuids.find(l.guid));
		if (r == remoteGuids.end())
		{
			NameMap::iterator r(remoteNames.find(l.name));
			if (r == remoteNames.end())
			{
				if (l.isDirty)
					actions.push_back(SyncAction<T>(SyncActionUpload, &l, NULL));
				else
					actions.push_back(SyncAction<T>(SyncActionDelete, &l, NULL));
			}
		}
	}
}

template<typename T>
void ComputeIncrementalSyncActions
	( const std::vector<T>    & remote
	, const std::vector<T>    & local
	, std::vector<SyncAction<T> > & actions
	)
{
	typedef std::map<std::string,  const T*> GuidMap;
	typedef std::map<std::wstring, const T*> NameMap;

	GuidMap localGuids;
	NameMap localNames;
	foreach (const T & l, local)
	{
		localGuids[l.guid] = &l;
		localNames[l.name] = &l;
	}

	GuidMap remoteGuids;
	NameMap remoteNames;
	foreach (const T & r, remote)
	{
		remoteGuids[r.guid] = &r;
		remoteNames[r.name] = &r;
	}

	foreach (const T & r, remote)
	{
		GuidMap::iterator l(localGuids.find(r.guid));
		if (l == localGuids.end())
		{
			NameMap::iterator l(localNames.find(r.name));
			if (l == localNames.end())
			{
				actions.push_back(SyncAction<T>(SyncActionAdd, NULL, &r));
			}
			else
			{
				const T & l(*l->second);
				if (l.isDirty)
				{
					actions.push_back(SyncAction<T>(SyncActionMerge, &l, &r));
				}
				else
				{
					actions.push_back(SyncAction<T>(SyncActionDelete, &l,   NULL));
					actions.push_back(SyncAction<T>(SyncActionAdd,    NULL, &r));
					// The official document advises renaming,
					// but that does not work well for notes.
					// Replacement seems to be a more sensible
					// approach in general.
					// actions.push_back(SyncAction<T>(ActionRenameAdd, &l, &r));
				}
			}
		}
		else
		{
			const T & l(*l->second);
			if (l.isDirty)
			{
				if (l.usn < r.usn)
					actions.push_back(SyncAction<T>(SyncActionMerge, &l, &r));
				else
					actions.push_back(SyncAction<T>(SyncActionUpload, &l, NULL));
			}
			else
			{
				actions.push_back(SyncAction<T>(SyncActionAdd, NULL, &r));
			}
		}
	}

	foreach (const T & l, local)
	{
		GuidMap::iterator r(remoteGuids.find(l.guid));
		if (r == remoteGuids.end())
		{
			NameMap::iterator r(remoteNames.find(l.name));
			if (r == remoteNames.end())
			{
				if (l.isDirty)
					actions.push_back(SyncAction<T>(SyncActionUpload, &l, NULL));
			}
		}
	}
}

template<typename T>
void ComputeSyncActions
	( bool                      fullSync
	, const std::vector<T>    & remote
	, const std::vector<T>    & local
	, std::vector<SyncAction<T> > & actions
	)
{
	if (local.empty() && remote.empty())
		return;
	if (fullSync)
		ComputeFullSyncActions(remote, local, actions);
	else
		ComputeIncrementalSyncActions(remote, local, actions);
}
