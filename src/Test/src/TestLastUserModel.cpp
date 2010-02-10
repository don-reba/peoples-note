#include "stdafx.h"
#include "LastUserModel.h"
#include "MockRegistryKey.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(LastUserModel_Test)
{
	MockRegistryKey key;
	LastUserModel   lastUserModel(key);

	key.data[L"username"] = L"test-usr";
	key.data[L"password"] = L"test-pwd";

	const ICredentialsModel & credentials = lastUserModel.GetCredentials();

	BOOST_CHECK_EQUAL(credentials.GetUsername(), L"test-usr");
	BOOST_CHECK_EQUAL(credentials.GetPassword(), L"test-pwd");
}
