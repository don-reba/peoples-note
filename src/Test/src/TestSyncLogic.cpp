#include "stdafx.h"
#include "SyncLogic.h"

#include "Guid.h"
#include "IResourceProcessor.h"
#include "MockSyncLogger.h"

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

class ResourceProcessor : public IResourceProcessor<MockResource>
{
public:

	virtual void Add(const MockResource & r)
	{
		r.status = StatusAdded;
	}

	virtual void Delete(const MockResource & l)
	{
		l.status = StatusDeleted;
	}

	virtual void Rename(const MockResource & l)
	{
		l.status = StatusRenamed;
	}

	virtual void Upload(const MockResource & l)
	{
		l.status = StatusUploaded;
	}

	virtual void Merge(const MockResource & l, const MockResource & r)
	{
		l.status = StatusMerged;
		r.status = StatusMerged;
	}
};

struct SyncLogicFixture
{
	MockSyncLogger    syncLogger;
	SyncLogic         syncLogic;
	ResourceProcessor resourceProcessor;

	vector<MockResource> local;
	vector<MockResource> remote;

	SyncLogicFixture()
		: syncLogic(syncLogger)
	{
	}

	void Sync()
	{
		syncLogic.FullSync(remote, local, resourceProcessor);
	}
};

//-----------
// test suite
//-----------

BOOST_FIXTURE_TEST_CASE(SyncLogic_Empty_Test, SyncLogicFixture)
{
	Sync();
}

BOOST_FIXTURE_TEST_CASE(SyncLogic_FullSync_Test, SyncLogicFixture)
{
	remote.push_back(MockResource(L"remote", Guid(), 0, false, StatusAdded));

	remote.push_back(MockResource(L"commonDirty", Guid(), 0, false, StatusMerged));
	local.push_back(MockResource(L"commonDirty", Guid(), 0, true, StatusMerged));

	remote.push_back(MockResource(L"commonFresh", Guid(), 0, false, StatusClear));
	local.push_back(MockResource(L"commonFresh", Guid(), 0, false, StatusRenamed));

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

	Sync();

	foreach (const MockResource & resource, local)
		BOOST_CHECK_EQUAL(resource.status, resource.goal);
	foreach (const MockResource & resource, remote)
		BOOST_CHECK_EQUAL(resource.status, resource.goal);
}
