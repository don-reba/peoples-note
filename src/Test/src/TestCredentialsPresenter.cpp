#include "stdafx.h"
#include "CredentialsPresenter.h"
#include "MockCredentialsModel.h"
#include "MockCredentialsView.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(CredentialsPresenter_Test)
{
	MockCredentialsModel model;
	MockCredentialsView  view;
	CredentialsPresenter presenter(model, view);

	view.username = L"test-usr";
	view.password = L"test-pwd";
	view.SignIn();

	BOOST_CHECK_EQUAL(model.GetUsername(), L"test-usr");
	BOOST_CHECK_EQUAL(model.GetPassword(), L"test-pwd");
}
