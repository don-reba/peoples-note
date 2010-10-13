#include "stdafx.h"
#include "MockCredentialsModel.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"
#include "SignalCheck.h"
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

BOOST_FIXTURE_TEST_CASE
	( UserSignInPresenter_UpdateNew_Test
	, UserSignInPresenterFixture
	)
{
	credentialsModel.SignalUpdated();
	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodNone);

	userModel.Load(L"[anonymous]");
	credentialsModel.username = L"test-usr";
	credentialsModel.password = L"test-pwd";
	credentialsModel.SignalUpdated();
	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodLoadAs);

	Credentials credentials;
	userModel.GetCredentials(credentials);
	BOOST_CHECK_EQUAL(credentials.GetUsername(), L"test-usr");
	BOOST_CHECK_EQUAL(credentials.GetPassword(), L"test-pwd");
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
	SignalCheck signalUpdatingCheck;
	credentialsModel.SignalUpdating.connect(boost::ref(signalUpdatingCheck));

	userModel.SetCredentials(L"[anonymous]", L"");

	noteListView.SignalSignIn();
	BOOST_CHECK(signalUpdatingCheck);
}

BOOST_FIXTURE_TEST_CASE
	( UserSignInPresenter_SignOut_Test
	, UserSignInPresenterFixture
	)
{
	SignalCheck signalUpdatingCheck;
	credentialsModel.SignalUpdating.connect(boost::ref(signalUpdatingCheck));

	noteListView.SignalSignIn();
	BOOST_CHECK(!signalUpdatingCheck);

	BOOST_CHECK_EQUAL(credentialsModel.GetUsername(), L"[anonymous]");
	BOOST_CHECK_EQUAL(credentialsModel.GetPassword(), L"");

	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodLoadOrCreate);
}
