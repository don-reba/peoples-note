// Implements the Evernote synchronization spec.
// The difference between full sync logic and incremental is that the former
// deletes clean strictly local notes.

#pragma once

#include "Guid.h"

#include <map>

class SyncLogic
{
public:

	enum ActionType
	{
		ActionAdd,    // add note from the server
		ActionDelete, // delete note on the server
		ActionUpload, // add note to the server
		ActionMerge,  // merge two conflicting notes

		// not implemented
		// ActionRenameAdd, // rename and add note from the server
	};

	template<typename T>
	struct Action
	{
		ActionType Type;
		const T  * Local;
		const T  * Remote;

		Action(ActionType type, const T * local, const T * remote);
	};

public:

	template<typename T>
	static void Sync
		( bool                      fullSync
		, const std::vector<T>    & remote
		, const std::vector<T>    & local
		, std::vector<Action<T> > & actions
		);

private:

	template<typename T>
	static void FullSync
		( const std::vector<T>    & remote
		, const std::vector<T>    & local
		, std::vector<Action<T> > & actions
		);

	template<typename T>
	static void IncrementalSync
		( const std::vector<T>    & remote
		, const std::vector<T>    & local
		, std::vector<Action<T> > & actions
		);
};

template<typename T>
SyncLogic::Action<T>::Action
	( ActionType type
	, const T  * local
	, const T  * remote
	)
	: Type   (type)
	, Local  (local)
	, Remote (remote)
{
}

template<typename T>
void SyncLogic::Sync
	( bool                      fullSync
	, const std::vector<T>    & remote
	, const std::vector<T>    & local
	, std::vector<Action<T> > & actions
	)
{
	if (local.empty() && remote.empty())
		return;
	if (fullSync)
		FullSync(remote, local, actions);
	else
		IncrementalSync(remote, local, actions);
}

template<typename T>
void SyncLogic::FullSync
	( const std::vector<T>    & remote
	, const std::vector<T>    & local
	, std::vector<Action<T> > & actions
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
				actions.push_back(Action<T>(ActionAdd, NULL, &r));
			}
			else
			{
				const T & l(*l->second);
				if (l.isDirty)
				{
					actions.push_back(Action<T>(ActionMerge, &l, &r));
				}
				else
				{
					actions.push_back(Action<T>(ActionDelete, &l,   NULL));
					actions.push_back(Action<T>(ActionAdd,    NULL, &r));
					// The official document advises renaming,
					// but that does not work well for notes.
					// Replacement seems to be a more sensible
					// approach in general.
					// actions.push_back(Action<T>(ActionRenameAdd, &l, &r));
				}
			}
		}
		else
		{
			const T & l(*l->second);
			if (l.isDirty)
			{
				if (l.usn != r.usn)
					actions.push_back(Action<T>(ActionMerge, &l, &r));
				else
					actions.push_back(Action<T>(ActionUpload, &l, NULL));
			}
			else
			{
				if (l.usn != r.usn)
					actions.push_back(Action<T>(ActionAdd, NULL, &r));
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
					actions.push_back(Action<T>(ActionUpload, &l, NULL));
				else
					actions.push_back(Action<T>(ActionDelete, &l, NULL));
			}
		}
	}
}

template<typename T>
void SyncLogic::IncrementalSync
	( const std::vector<T>    & remote
	, const std::vector<T>    & local
	, std::vector<Action<T> > & actions
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
				actions.push_back(Action<T>(ActionAdd, NULL, &r));
			}
			else
			{
				const T & l(*l->second);
				if (l.isDirty)
				{
					actions.push_back(Action<T>(ActionMerge, &l, &r));
				}
				else
				{
					actions.push_back(Action<T>(ActionDelete, &l,   NULL));
					actions.push_back(Action<T>(ActionAdd,    NULL, &r));
					// The official document advises renaming,
					// but that does not work well for notes.
					// Replacement seems to be a more sensible
					// approach in general.
					// actions.push_back(Action<T>(ActionRenameAdd, &l, &r));
				}
			}
		}
		else
		{
			const T & l(*l->second);
			if (l.isDirty)
			{
				if (l.usn < r.usn)
					actions.push_back(Action<T>(ActionMerge, &l, &r));
				else
					actions.push_back(Action<T>(ActionUpload, &l, NULL));
			}
			else
			{
				actions.push_back(Action<T>(ActionAdd, NULL, &r));
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
					actions.push_back(Action<T>(ActionUpload, &l, NULL));
			}
		}
	}
}
