#include "stdafx.h"
#include "UserLoader.h"
#include "MockLastUserModel.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

struct UserLoaderFixture
{
	MockUserModel     userModel;
	MockLastUserModel lastUserModel;
	UserLoader        userLoader;

	UserLoaderFixture() : userLoader(userModel, lastUserModel) {}
};

BOOST_FIXTURE_TEST_CASE
	( UserLoader_DefaultUser_Test
	, UserLoaderFixture
	)
{
	userLoader.Run();

	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodLoadOrCreate);
	BOOST_CHECK_EQUAL(userModel.loadedAs, L"[anonymous]");
}

BOOST_FIXTURE_TEST_CASE
	( UserLoader_Test
	, UserLoaderFixture
	)
{
	lastUserModel.username = L"test-usr";

	userLoader.Run();

	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodLoad);

	BOOST_CHECK_EQUAL(userModel.loadedAs, L"test-usr");
}

BOOST_FIXTURE_TEST_CASE
	( UserLoader_Anonymous_Test
	, UserLoaderFixture
	)
{
	userLoader.Run();

	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodLoadOrCreate);

	BOOST_CHECK_EQUAL(userModel.loadedAs, L"[anonymous]");
}
