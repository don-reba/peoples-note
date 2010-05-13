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

	template<typename T>
	void IncrementalSync
		( const std::vector<T>  & remote
		, const std::vector<T>  & local
		, IResourceProcessor<T> & processor
		);
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
					syncLogger.Add(r.guid);
					processor.Add(r);
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

template<typename T>
void SyncLogic::IncrementalSync
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
					syncLogger.Add(r.guid);
					processor.Add(r);
				}
			}
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
				syncLogger.Add(r.guid);
				processor.Add(r);
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
			}
		}
	}
}
