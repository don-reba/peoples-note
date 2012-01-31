#include "stdafx.h"
#include "LastUserModel.h"

#include "MockRegistryKey.h"

using namespace boost;
using namespace std;

struct LastUserModelFixture
{
	MockRegistryKey registryKey;

	LastUserModel lastUserModel;

	LastUserModelFixture()
		: lastUserModel(registryKey)
	{
	}
};

BOOST_FIXTURE_TEST_CASE(LastUserModel_Get, LastUserModelFixture)
{
	registryKey.SetString(L"password", L"password");
	registryKey.SetString(L"username", L"username");

	BOOST_CHECK_EQUAL(lastUserModel.GetUsername(), L"username");
	BOOST_CHECK_EQUAL(lastUserModel.GetPassword(), L"password");
}

BOOST_FIXTURE_TEST_CASE(LastUserModel_Set, LastUserModelFixture)
{
	lastUserModel.SetCredentials(L"username", L"password");

	BOOST_CHECK_EQUAL(registryKey.GetString(L"username", L""), L"username");
	BOOST_CHECK_EQUAL(registryKey.GetString(L"password", L""), L"password");
}
