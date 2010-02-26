#include "stdafx.h"

#include "MockDataStore.h"
#include "UserModel.h"

using namespace boost;
using namespace std;

/*
BOOST_AUTO_TEST_CASE(UserModel_CreateDefaultUser_Test)
{
	MockDataStore dataStore;
	UserModel userModel(dataStore);
	userModel.CreateDefaultUser();

	vector<INotebook*> notebooks = userModel.GetNotebooks();
	BOOST_REQUIRE_EQUAL(notebooks.size(), 1);

	INotebook & notebook = *notebooks.at(0);
	BOOST_CHECK_EQUAL(notebook.GetName(), L"Notes");
}
*/
