#include "stdafx.h"
#include "LastUserModel.h"
#include "MockRegistryKey.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(LastUserModel_Get_Test)
{
	MockRegistryKey key;
	LastUserModel   lastUserModel(key);

	key.SetString(L"username", L"test-usr");

	BOOST_CHECK_EQUAL(lastUserModel.GetUsername(), L"test-usr");
}

BOOST_AUTO_TEST_CASE(LastUserModel_Set_Test)
{
	MockRegistryKey key;
	LastUserModel   lastUserModel(key);

	lastUserModel.SetUsername(L"test-usr");

	BOOST_CHECK_EQUAL(key.GetString(L"username", L""), L"test-usr");
}
