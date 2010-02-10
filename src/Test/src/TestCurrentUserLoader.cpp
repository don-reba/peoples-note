#include "stdafx.h"
#include "CurrentUserLoader.h"
#include "MockLastUserModel.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(CurrentUserLoader_DefaultUser_Test)
{
	MockUserModel     userModel;
	MockLastUserModel lastUserModel;
	MockNoteListView  noteListView;
	CurrentUserLoader(userModel, lastUserModel, noteListView);

	noteListView.SignalCreated();

	BOOST_CHECK(userModel.isDefault);
	BOOST_CHECK(userModel.isLoaded);
}

BOOST_AUTO_TEST_CASE(CurrentUserLoader_Test)
{
	MockUserModel     userModel;
	MockLastUserModel lastUserModel;
	MockNoteListView  noteListView;
	CurrentUserLoader(userModel, lastUserModel, noteListView);

	lastUserModel.credentialsModel.username = L"test-usr";
	lastUserModel.credentialsModel.password = L"test-pwd";

	noteListView.SignalCreated();

	BOOST_CHECK(!userModel.isDefault);
	BOOST_CHECK(userModel.isLoaded);

	BOOST_CHECK_EQUAL(userModel.credentialsModel.username, L"test-usr");
	BOOST_CHECK_EQUAL(userModel.credentialsModel.password, L"test-pwd");
}

BOOST_AUTO_TEST_CASE(CurrentUserLoader_NoPassword_Test)
{
	MockUserModel     userModel;
	MockLastUserModel lastUserModel;
	MockNoteListView  noteListView;
	CurrentUserLoader(userModel, lastUserModel, noteListView);

	lastUserModel.credentialsModel.username = L"test-usr";
	lastUserModel.credentialsModel.password = L"";

	noteListView.SignalCreated();

	BOOST_CHECK(!userModel.isDefault);
	BOOST_CHECK(userModel.isLoaded);

	BOOST_CHECK_EQUAL(userModel.credentialsModel.username, L"test-usr");
	BOOST_CHECK_EQUAL(userModel.credentialsModel.password, L"");
}
