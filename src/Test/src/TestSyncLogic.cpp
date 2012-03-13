#include "stdafx.h"
#include "SyncLogic.h"

#include "Guid.h"
#include "IResourceProcessor.h"

using namespace boost;
using namespace std;

//----------------------
// auxillary definitions
//----------------------

enum Status
{
	StatusAdded,
	StatusDeleted,
	StatusRenamed,
	StatusUploaded,
	StatusMerged,
	StatusClear,
};

namespace std
{
	ostream & operator << (ostream & stream, Status status)
	{
		switch (status)
		{
		case StatusAdded:    return stream << "Added";
		case StatusDeleted:  return stream << "Deleted";
		case StatusRenamed:  return stream << "Renamed";
		case StatusUploaded: return stream << "Uploaded";
		case StatusMerged:   return stream << "Merged";
		case StatusClear:    return stream << "Clear";
		}
		return stream;
	}
}

class MockResource
{
public:

	wstring name;
	Guid    guid;
	int     usn;
	bool    isDirty;

	Status goal;

	mutable Status status;

public:

	MockResource
		( wstring name
		, Guid    guid
		, int     usn
		, bool    isDirty
		, Status  goal
		)
		: name    (name)
		, guid    (guid)
		, usn     (usn)
		, isDirty (isDirty)
		, goal    (goal)
		, status  (StatusClear)
	{
	}

	wstring GetName() const
	{
		return name;
	}

	Guid GetGuid() const
	{
		return guid;
	}

	int GetUsn() const
	{
		return usn;
	}

	bool IsDirty() const
	{
		return isDirty;
	}
};

struct SyncLogicFixture
{
	vector<MockResource> local;
	vector<MockResource> remote;

	vector<SyncAction<MockResource> > actions;

	void FullSync()
	{
		ComputeSyncActions(true, remote, local, actions);
		ProcessActions();
	}

	void IncrementalSync()
	{
		ComputeSyncActions(false, remote, local, actions);
		ProcessActions();
	}

	void ProcessActions()
	{
		foreach (const SyncAction<MockResource> action, actions)
		{
			switch (action.Type)
			{
			case SyncActionAdd:
				action.Remote->status = StatusAdded;
				break;
			case SyncActionDelete:
				action.Local->status = StatusDeleted;
				break;
			case SyncActionMerge:
				action.Local->status  = StatusMerged;
				action.Remote->status = StatusMerged;
				break;
			case SyncActionUpload:
				action.Local->status = StatusUploaded;
				break;
			}
		}
	}
};

//-----------
// test suite
//-----------

BOOST_FIXTURE_TEST_CASE(SyncLogic_Empty_Test, SyncLogicFixture)
{
	FullSync();

	BOOST_CHECK(local.empty());
	BOOST_CHECK(remote.empty());

	IncrementalSync();

	BOOST_CHECK(local.empty());
	BOOST_CHECK(remote.empty());
}

BOOST_FIXTURE_TEST_CASE(SyncLogic_FullSync_Test, SyncLogicFixture)
{
	remote.push_back(MockResource(L"remote", Guid(), 0, false, StatusAdded));

	remote.push_back(MockResource(L"commonDirty", Guid(), 0, false, StatusMerged));
	local.push_back(MockResource(L"commonDirty", Guid(), 0, true, StatusMerged));

	remote.push_back(MockResource(L"commonFresh", Guid(), 0, false, StatusAdded));
	local.push_back(MockResource(L"commonFresh", Guid(), 0, false, StatusDeleted));

	Guid guid0;
	remote.push_back(MockResource(L"0", guid0, 2, false, StatusClear));
	local.push_back(MockResource(L"0", guid0, 2, true, StatusUploaded));

	Guid guid1;
	remote.push_back(MockResource(L"1", guid1, 2, false, StatusClear));
	local.push_back(MockResource(L"2", guid1, 2, false, StatusClear));

	Guid guid2;
	remote.push_back(MockResource(L"3", guid2, 3, false, StatusMerged));
	local.push_back(MockResource(L"4", guid2, 2, true, StatusMerged));

	Guid guid3;
	remote.push_back(MockResource(L"5", guid3, 3, false, StatusAdded));
	local.push_back(MockResource(L"6", guid3, 2, false, StatusClear));

	local.push_back(MockResource(L"7", Guid(), 0, true, StatusUploaded));

	local.push_back(MockResource(L"8", Guid(), 0, false, StatusDeleted));

	FullSync();

	foreach (const MockResource & resource, local)
		BOOST_CHECK_EQUAL(resource.status, resource.goal);
	foreach (const MockResource & resource, remote)
		BOOST_CHECK_EQUAL(resource.status, resource.goal);
}

BOOST_FIXTURE_TEST_CASE(SyncLogic_IncrementalSyncTest, SyncLogicFixture)
{
	remote.push_back(MockResource(L"remote", Guid(), 0, false, StatusAdded));

	remote.push_back(MockResource(L"0", Guid(), 0, false, StatusMerged));
	local.push_back(MockResource(L"0", Guid(), 0, true, StatusMerged));

	remote.push_back(MockResource(L"1", Guid(), 0, false, StatusAdded));
	local.push_back(MockResource(L"1", Guid(), 0, false, StatusDeleted));

	Guid guid0;

	remote.push_back(MockResource(L"2", guid0, 0, false, StatusClear));
	local.push_back(MockResource(L"3", guid0, 0, true, StatusUploaded));

	Guid guid1;
	remote.push_back(MockResource(L"4", guid1, 0, false, StatusAdded));
	local.push_back(MockResource(L"5", guid1, 0, false, StatusClear));

	local.push_back(MockResource(L"6", Guid(), 0, true, StatusUploaded));

	local.push_back(MockResource(L"7", Guid(), 0, false, StatusClear));

	Guid guid2;
	remote.push_back(MockResource(L"2", guid2, 1, false, StatusMerged));
	local.push_back(MockResource(L"3", guid2, 0, true, StatusMerged));

	IncrementalSync();

	foreach (const MockResource & resource, local)
		BOOST_CHECK_EQUAL(resource.status, resource.goal);
	foreach (const MockResource & resource, remote)
		BOOST_CHECK_EQUAL(resource.status, resource.goal);
}
