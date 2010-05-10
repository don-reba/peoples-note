#pragma once

#include "Guid.h"
#include "IResourceProcessor.h"
#include "ISyncLogger.h"

#include <map>

class SyncLogic
{
private:

	ISyncLogger & syncLogger;

public:

	SyncLogic(ISyncLogger & syncLogger)
		: syncLogger (syncLogger)
	{
	}

	template<typename T>
	void FullSync
		( const std::vector<T>  & remote
		, const std::vector<T>  & local
		, IResourceProcessor<T> & processor
		);

#if 0
	template<typename T>
	void IncrementalSync
		( std::vector<T> & remote
		, std::vector<T> & local
		, void (*Add)(T&)
		, void (*Delete)(T&)
		, void (*Rename)(T&)
		, void (*Upload)(T&)
		, void (*Merge)(T&, T&)
		);
#endif // 0
};

template<typename T>
void SyncLogic::FullSync
	( const std::vector<T>  & remote
	, const std::vector<T>  & local
	, IResourceProcessor<T> & processor
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
				syncLogger.Add(r.guid);
				processor.Add(r);
			}
			else
			{
				const T & l(*l->second);
				if (l.isDirty)
				{
					syncLogger.Merge(l.guid, r.guid);
					processor.Merge(l, r);
				}
				else
				{
					syncLogger.Rename(l.guid);
					processor.Rename(l);
				}
			}
		}
		else
		{
			const T & l(*l->second);
			if (l.usn == r.usn)
			{
				if (l.isDirty)
				{
					syncLogger.Upload(l.guid);
					processor.Upload(l);
				}
			}
			else
			{
				if (l.isDirty)
				{
					syncLogger.Merge(l.guid, r.guid);
					processor.Merge(l, r);
				}
				else
				{
					syncLogger.Add(r.guid);
					processor.Add(r);
				}
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
				{
					syncLogger.Upload(l.guid);
					processor.Upload(l);
				}
				else
				{
					syncLogger.Delete(l.guid);
					processor.Delete(l);
				}
			}
		}
	}
}

#if 0
template<typename T>
void SyncLogic::IncrementalSync
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
		localGuids[l.guid] = &l;

	Map remoteGuids;
	foreach (T & r, remote)
		remoteGuids[r.guid] = &r;

	foreach (T & r, remote)
	{
		Map::iterator l(localGuids.find(r.guid));
		if (l == localGuids.end())
		{
			Add(r);
		}
		else
		{
			T & l(*l->second);
			if (l.name == r.name)
			{
				if (l.isDirty)
					Merge(l, r);
				else
					Rename(l);
			}
			else
			{
				if (l.isDirty)
					Merge(l, r);
				else
					Add(r);
			}
		}
	}

	foreach (T & l, local)
	{
		Map::iterator r(remoteGuids.find(l.guid));
		if (r == remoteGuids.end())
		{
			if (l.isDirty)
				Upload(l);
			else
				Delete(l);
		}
	}
}
#endif // 0
