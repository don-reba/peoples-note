#include "stdafx.h"
#include "CredentialsPresenter.h"

#include "MockCredentialsModel.h"
#include "MockCredentialsView.h"
#include "MockEnService.h"

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

struct CredentialsPresenterFixture
{
	MockCredentialsModel credentialsModel;
	MockCredentialsView  credentialsView;

	CredentialsPresenter credentialsPresenter;

	CredentialsPresenterFixture()
		: credentialsPresenter
			( credentialsModel
			, credentialsView
			)
	{
	}
};

//-----------
// test cases
//-----------

BOOST_FIXTURE_TEST_CASE
	( CredentialsPresenter_Ok
	, CredentialsPresenterFixture
	)
{
	credentialsView.username = L"username";
	credentialsView.password = L"password";

	credentialsView.SignalOk();

	BOOST_CHECK_EQUAL(credentialsModel.username, L"username");
	BOOST_CHECK_EQUAL(credentialsModel.password, L"password");
}

BOOST_FIXTURE_TEST_CASE
	( CredentialsPresenter_Cancel
	, CredentialsPresenterFixture
	)
{
	credentialsModel.username = L"fake";
	credentialsModel.password = L"fake";

	credentialsView.SignalCancel();

	BOOST_CHECK_EQUAL(credentialsModel.username, L"[anonymous]");
	BOOST_CHECK_EQUAL(credentialsModel.password, L"");
}

BOOST_FIXTURE_TEST_CASE
	( CredentialsPresenter_Commit
	, CredentialsPresenterFixture
	)
{
	credentialsView.isOpen = true;

	credentialsModel.Commit();

	BOOST_CHECK(!credentialsView.isOpen);
}

BOOST_FIXTURE_TEST_CASE
	( CredentialsPresenter_Update
	, CredentialsPresenterFixture
	)
{
	credentialsModel.username = L"username";
	credentialsModel.password = L"password";
	credentialsModel.status   = L"status";

	credentialsModel.SignalUpdate();

	BOOST_CHECK(credentialsView.isOpen);
	BOOST_CHECK_EQUAL(credentialsView.username, L"username");
	BOOST_CHECK_EQUAL(credentialsView.password, L"");
	BOOST_CHECK_EQUAL(credentialsView.message,  L"status");
	BOOST_CHECK(!credentialsView.isUsernameFocused);
	BOOST_CHECK(credentialsView.isPasswordFocused);

	credentialsModel.username = L"";

	credentialsModel.SignalUpdate();

	BOOST_CHECK(credentialsView.isUsernameFocused);
	BOOST_CHECK(!credentialsView.isPasswordFocused);
}

BOOST_FIXTURE_TEST_CASE
	( CredentialsPresenter_ViewCreated
	, CredentialsPresenterFixture
	)
{
	credentialsModel.username = L"username";
	credentialsModel.password = L"password";

	credentialsView.SignalCreated();

	BOOST_CHECK_EQUAL(credentialsView.username, L"username");
	BOOST_CHECK_EQUAL(credentialsView.password, L"");

	credentialsModel.username = L"[anonymous]";

	credentialsView.SignalCreated();

	BOOST_CHECK_EQUAL(credentialsView.username, L"");
	BOOST_CHECK_EQUAL(credentialsView.password, L"");
}
