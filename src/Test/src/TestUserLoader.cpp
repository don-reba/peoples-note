#include "stdafx.h"
#include "UserLoader.h"
#include "MockLastUserModel.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(UserLoader_DefaultUser_Test)
{
	MockUserModel     userModel;
	MockLastUserModel lastUserModel;
	UserLoader userLoader(userModel, lastUserModel);

	userLoader.Run();

	BOOST_CHECK(userModel.isDefault);
	BOOST_CHECK(userModel.isLoaded);
}

BOOST_AUTO_TEST_CASE(UserLoader_Test)
{
	MockUserModel     userModel;
	MockLastUserModel lastUserModel;
	UserLoader userLoader(userModel, lastUserModel);

	lastUserModel.credentialsModel.username = L"test-usr";
	lastUserModel.credentialsModel.password = L"test-pwd";

	userLoader.Run();

	BOOST_CHECK(!userModel.isDefault);
	BOOST_CHECK(userModel.isLoaded);

	BOOST_CHECK_EQUAL(userModel.credentialsModel.username, L"test-usr");
	BOOST_CHECK_EQUAL(userModel.credentialsModel.password, L"test-pwd");
}

BOOST_AUTO_TEST_CASE(UserLoader_NoPassword_Test)
{
	MockUserModel     userModel;
	MockLastUserModel lastUserModel;
	UserLoader userLoader(userModel, lastUserModel);

	lastUserModel.credentialsModel.username = L"test-usr";
	lastUserModel.credentialsModel.password = L"";

	userLoader.Run();

	BOOST_CHECK(!userModel.isDefault);
	BOOST_CHECK(userModel.isLoaded);

	BOOST_CHECK_EQUAL(userModel.credentialsModel.username, L"test-usr");
	BOOST_CHECK_EQUAL(userModel.credentialsModel.password, L"");
}
