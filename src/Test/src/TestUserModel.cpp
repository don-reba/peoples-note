#include "stdafx.h"

#include "MockDataStore.h"
#include "UserModel.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(UserModel_CreateDefaultUser_Test)
{
	MockDataStore dataStore;
	UserModel userModel(dataStore);
	userModel.CreateDefaultUser();
	userModel.Load();

	BOOST_CHECK_EQUAL(dataStore.name, L"");
	BOOST_CHECK_EQUAL(dataStore.GetLastUsedNotebook().GetName(), L"Notes");

	const NotebookList & notebooks = userModel.GetNotebooks();
	BOOST_REQUIRE_EQUAL(notebooks.size(), 1);
	BOOST_CHECK_EQUAL(notebooks.at(0).GetName(), L"Notes");
}
