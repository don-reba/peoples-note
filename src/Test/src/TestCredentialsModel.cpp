#include "stdafx.h"
#include "CredentialsModel.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(CredentialsModel_Test)
{
	CredentialsModel model;
	model.SetCredentials(L"test-usr", L"test-pwd");

	BOOST_CHECK_EQUAL(model.GetUsername(), L"test-usr");
	BOOST_CHECK_EQUAL(model.GetPassword(), L"test-pwd");
}
