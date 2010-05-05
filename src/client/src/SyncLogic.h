#pragma once

#include "Guid.h"
#include "IResourceProcessor.h"

#include <map>

class SyncLogic
{
public:

	template<typename T>
	void FullSync
		( const std::vector<T>  & remote
		, const std::vector<T>  & local
		, IResourceProcessor<T> & processor
		);
	//template<typename T>
	//void IncrementalSync
	//	( std::vector<T> & remote
	//	, std::vector<T> & local
	//	, void (*Add)(T&)
	//	, void (*Delete)(T&)
	//	, void (*Rename)(T&)
	//	, void (*Upload)(T&)
	//	, void (*Merge)(T&, T&)
	//	);
};

template<typename T>
void SyncLogic::FullSync
	( const std::vector<T>  & remote
	, const std::vector<T>  & local
	, IResourceProcessor<T> & processor
	)
{
	typedef std::map<std::string, const T*> Map;

	Map localGuids;
	foreach (const T & l, local)
		localGuids[l.guid] = &l;

	Map remoteGuids;
	foreach (const T & r, remote)
		remoteGuids[r.guid] = &r;

	foreach (const T & r, remote)
	{
		Map::iterator l(localGuids.find(r.guid));
		if (l == localGuids.end())
		{
			processor.Add(r);
		}
		else
		{
			const T & l(*l->second);
			if (l.name == r.name)
			{
				if (l.isDirty)
					processor.Merge(l, r);
				else
					processor.Rename(l);
			}
			else
			{
				if (l.usn == r.usn)
				{
					if (l.isDirty)
						processor.Upload(l);
				}
				else
				{
					if (l.isDirty)
						processor.Merge(l, r);
					else
						processor.Add(r);
				}
			}
		}
	}

	foreach (const T & l, local)
	{
		Map::iterator r(remoteGuids.find(l.guid));
		if (r == remoteGuids.end())
		{
			if (l.isDirty)
				processor.Upload(l);
			else
				processor.Delete(l);
		}
	}
}

//template<typename T>
//void SyncLogic::IncrementalSync
//	( std::vector<T> & remote
//	, std::vector<T> & local
//	, void (*Add)(T&)
//	, void (*Delete)(T&)
//	, void (*Rename)(T&)
//	, void (*Upload)(T&)
//	, void (*Merge)(T&, T&)
//	)
//{
//	typedef std::map<std::string, T*> Map;
//
//	Map localGuids;
//	foreach (T & l, local)
//		localGuids[l.guid] = &l;
//
//	Map remoteGuids;
//	foreach (T & r, remote)
//		remoteGuids[r.guid] = &r;
//
//	foreach (T & r, remote)
//	{
//		Map::iterator l(localGuids.find(r.guid));
//		if (l == localGuids.end())
//		{
//			Add(r);
//		}
//		else
//		{
//			T & l(*l->second);
//			if (l.name == r.name)
//			{
//				if (l.isDirty)
//					Merge(l, r);
//				else
//					Rename(l);
//			}
//			else
//			{
//				if (l.isDirty)
//					Merge(l, r);
//				else
//					Add(r);
//			}
//		}
//	}
//
//	foreach (T & l, local)
//	{
//		Map::iterator r(remoteGuids.find(l.guid));
//		if (r == remoteGuids.end())
//		{
//			if (l.isDirty)
//				Upload(l);
//			else
//				Delete(l);
//		}
//	}
//}
