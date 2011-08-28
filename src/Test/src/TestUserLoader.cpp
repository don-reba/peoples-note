#include "stdafx.h"
#include "UserLoader.h"
#include "MockLastUserModel.h"
#include "MockNoteListView.h"
#include "MockCredentialsModel.h"

using namespace boost;
using namespace std;

struct UserLoaderFixture
{
	MockCredentialsModel credentialsModel;
	MockLastUserModel    lastUserModel;

	UserLoader userLoader;

	UserLoaderFixture()
		: userLoader(credentialsModel, lastUserModel)
	{
	}
};

BOOST_FIXTURE_TEST_CASE
	( UserLoader_Run
	, UserLoaderFixture
	)
{
	lastUserModel.username = L"username";
	lastUserModel.password = L"password";

	userLoader.Run();

	BOOST_CHECK_EQUAL(credentialsModel.username, L"username");
	BOOST_CHECK_EQUAL(credentialsModel.password, L"password");
}

BOOST_FIXTURE_TEST_CASE
	( UserLoader_Commit
	, UserLoaderFixture
	)
{
	credentialsModel.username = L"username";
	credentialsModel.password = L"password";

	credentialsModel.Commit();

	BOOST_CHECK_EQUAL(lastUserModel.username, L"username");
	BOOST_CHECK_EQUAL(lastUserModel.password, L"password");
}
