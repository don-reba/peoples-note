#pragma once

#include "Guid.h"

#include <map>

class SyncLogic
{
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
		if (l == localGuids.end())
		{
			Add(r);
		}
		else
		{
			T & l(*l->second);
			if (l.GetName() == r.GetName())
			{
				if (l.IsDirty())
					Merge(l, r);
				else
					Rename(l);
			}
			else
			{
				if (l.GetUsn() == r.GetUsn())
				{
					if (l.IsDirty())
						Upload(l);
				}
				else
				{
					if (l.IsDirty())
						Merge(l, r);
					else
						Add(r);
				}
			}
		}
	}

	foreach (T & l, local)
	{
		Map::iterator r(remoteGuids.find(l.GetGuid()));
		if (r == remoteGuids.end())
		{
			if (l.IsDirty())
				Upload(l);
			else
				Delete(l);
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
//		localGuids[l.GetGuid()] = &l;
//
//	Map remoteGuids;
//	foreach (T & r, remote)
//		remoteGuids[r.GetGuid()] = &r;
//
//	foreach (T & r, remote)
//	{
//		Map::iterator l(localGuids.find(r.GetGuid()));
//		if (l == localGuids.end())
//		{
//			Add(r);
//		}
//		else
//		{
//			T & l(*l->second);
//			if (l.GetName() == r.GetName())
//			{
//				if (l.IsDirty())
//					Merge(l, r);
//				else
//					Rename(l);
//			}
//			else
//			{
//				if (l.IsDirty())
//					Merge(l, r);
//				else
//					Add(r);
//			}
//		}
//	}
//
//	foreach (T & l, local)
//	{
//		Map::iterator r(remoteGuids.find(l.GetGuid()));
//		if (r == remoteGuids.end())
//		{
//			if (l.IsDirty())
//				Upload(l);
//			else
//				Delete(l);
//		}
//	}
//}
