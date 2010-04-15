#include "stdafx.h"
#include "MockCredentialsModel.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"
#include "UserSignInPresenter.h"

#include <boost/ref.hpp>

struct UserSignInPresenterFixture
{
	MockCredentialsModel credentialsModel;
	MockNoteListView     noteListView;
	MockUserModel        userModel;
	UserSignInPresenter  signInPresenter;

	UserSignInPresenterFixture()
		: signInPresenter (credentialsModel, noteListView, userModel)
	{
	}
};

struct SignalCheck
{
	bool signalled;
	SignalCheck() : signalled(false) {}
	void operator () () { signalled = true; }
};

BOOST_FIXTURE_TEST_CASE
	( UserSignInPresenter_UpdateNew_Test
	, UserSignInPresenterFixture
	)
{
	credentialsModel.SignalUpdated();
	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodNone);

	userModel.Load(L"[anonymous]");
	credentialsModel.username = L"test-usr";
	credentialsModel.SignalUpdated();
	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodLoadAs);
}

BOOST_FIXTURE_TEST_CASE
	( UserSignInPresenter_UpdateOld_Test
	, UserSignInPresenterFixture
	)
{
	credentialsModel.SignalUpdated();
	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodNone);

	userModel.Load(L"[anonymous]");
	userModel.validUsernames.insert(L"test-usr");
	credentialsModel.username = L"test-usr";
	credentialsModel.SignalUpdated();
	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodLoad);
}

BOOST_FIXTURE_TEST_CASE
	( UserSignInPresenter_SignIn_Test
	, UserSignInPresenterFixture
	)
{
	SignalCheck check;
	credentialsModel.SignalUpdating.connect(boost::ref(check));

	userModel.GetCredentials().SetCredentials(L"[anonymous]", L"");

	noteListView.SignalSignIn();
	BOOST_CHECK(check.signalled);
}

BOOST_FIXTURE_TEST_CASE
	( UserSignInPresenter_SignOut_Test
	, UserSignInPresenterFixture
	)
{
	SignalCheck check;
	credentialsModel.SignalUpdating.connect(boost::ref(check));

	noteListView.SignalSignIn();
	BOOST_CHECK(!check.signalled);

	BOOST_CHECK_EQUAL(credentialsModel.GetUsername(), L"[anonymous]");
	BOOST_CHECK_EQUAL(credentialsModel.GetPassword(), L"");

	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodLoadOrCreate);
}
