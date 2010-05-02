#include "stdafx.h"
#include "SyncLogic.h"

#include "Guid.h"
#include "IResourceProcessor.h"

using namespace boost;
using namespace std;

//----------------------
// auxillary definitions
//----------------------

class MockResource
{
public:

	enum Status
	{
		StatusAdded,
		StatusDeleted,
		StatusRenamed,
		StatusUploaded,
		StatusMerged,
		StatusClear,
	};

public:

	mutable wstring name;
	mutable Guid    guid;
	mutable int     usn;
	mutable bool    isDirty;
	mutable Status  status;

public:

	MockResource()
		: usn     (0)
		, isDirty (false)
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
		r.status = MockResource::StatusAdded;
	}

	virtual void Delete(const MockResource & l)
	{
		l.status = MockResource::StatusDeleted;
	}

	virtual void Rename(const MockResource & l)
	{
		l.status = MockResource::StatusRenamed;
	}

	virtual void Upload(const MockResource & l)
	{
		l.status = MockResource::StatusUploaded;
	}

	virtual void Merge(const MockResource & l, const MockResource & r)
	{
		l.status = MockResource::StatusMerged;
		r.status = MockResource::StatusMerged;
	}
};

//-----------
// test suite
//-----------

BOOST_AUTO_TEST_CASE(SyncLogic_Empty_Test)
{
	SyncLogic         syncLogic;
	ResourceProcessor resourceProcessor;

	vector<MockResource> local;
	vector<MockResource> remote;

	syncLogic.FullSync(remote, local, resourceProcessor);
}

BOOST_AUTO_TEST_CASE(SyncLogic_FullSyncNoConflict_Test)
{
	SyncLogic         syncLogic;
	ResourceProcessor resourceProcessor;

	vector<MockResource> local;
	local.push_back(MockResource());
	local.push_back(MockResource());

	vector<MockResource> remote;
	remote.push_back(MockResource());

	local.at(0).isDirty = true;

	syncLogic.FullSync(remote, local, resourceProcessor);

	BOOST_CHECK_EQUAL(local.at(0).status, MockResource::StatusUploaded);
	BOOST_CHECK_EQUAL(local.at(1).status, MockResource::StatusDeleted);

	BOOST_CHECK_EQUAL(remote.at(0).status, MockResource::StatusAdded);
}

BOOST_AUTO_TEST_CASE(SyncLogic_FullSyncBasicMerge_Test)
{
	SyncLogic         syncLogic;
	ResourceProcessor resourceProcessor;

	vector<MockResource> local;
	local.push_back(MockResource());
	local.push_back(MockResource());

	vector<MockResource> remote;
	remote.push_back(MockResource());
	remote.push_back(MockResource());

	remote.at(0).guid = local.at(1).guid;
	local.at(1).name = L"other";

	syncLogic.FullSync(remote, local, resourceProcessor);

	BOOST_CHECK_EQUAL(local.at(0).status, MockResource::StatusDeleted);
	BOOST_CHECK_EQUAL(local.at(1).status, MockResource::StatusClear);

	BOOST_CHECK_EQUAL(remote.at(0).status, MockResource::StatusClear);
	BOOST_CHECK_EQUAL(remote.at(1).status, MockResource::StatusAdded);
}

BOOST_AUTO_TEST_CASE(SyncLogic_FullSyncConflict_Test)
{
	SyncLogic         syncLogic;
	ResourceProcessor resourceProcessor;

	vector<MockResource> local;
	local.push_back(MockResource());
	local.push_back(MockResource());
	local.push_back(MockResource());
	local.push_back(MockResource());
	local.push_back(MockResource());
	local.push_back(MockResource());

	vector<MockResource> remote;
	remote.push_back(MockResource());
	remote.push_back(MockResource());
	remote.push_back(MockResource());
	remote.push_back(MockResource());
	remote.push_back(MockResource());
	remote.push_back(MockResource());

	local.at(0).isDirty = true;
	local.at(2).isDirty = true;
	local.at(4).isDirty = true;

	remote.at(4).usn = 1;
	remote.at(5).usn = 1;

	remote.at(2).name = L"other";
	remote.at(3).name = L"other";
	remote.at(4).name = L"other";
	remote.at(5).name = L"other";

	for (int i = 0; i != min(local.size(), remote.size()); ++i)
		remote.at(i).guid = local.at(i).guid;

	syncLogic.FullSync(remote, local, resourceProcessor);

	BOOST_CHECK_EQUAL(local.at(0).status, MockResource::StatusMerged);
	BOOST_CHECK_EQUAL(local.at(1).status, MockResource::StatusRenamed);
	BOOST_CHECK_EQUAL(local.at(2).status, MockResource::StatusUploaded);
	BOOST_CHECK_EQUAL(local.at(3).status, MockResource::StatusClear);
	BOOST_CHECK_EQUAL(local.at(4).status, MockResource::StatusMerged);
	BOOST_CHECK_EQUAL(local.at(5).status, MockResource::StatusClear);

	BOOST_CHECK_EQUAL(remote.at(0).status, MockResource::StatusMerged);
	BOOST_CHECK_EQUAL(remote.at(1).status, MockResource::StatusClear);
	BOOST_CHECK_EQUAL(remote.at(2).status, MockResource::StatusClear);
	BOOST_CHECK_EQUAL(remote.at(3).status, MockResource::StatusClear);
	BOOST_CHECK_EQUAL(remote.at(4).status, MockResource::StatusMerged);
	BOOST_CHECK_EQUAL(remote.at(5).status, MockResource::StatusAdded);
}
