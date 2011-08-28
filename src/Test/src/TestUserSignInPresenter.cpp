#include "stdafx.h"
#include "MockCredentialsModel.h"
#include "MockEnService.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"
#include "SignalCheck.h"
#include "Tools.h"
#include "UserSignInPresenter.h"

#include <boost/ref.hpp>

using namespace boost;
using namespace Tools;

//-----------------------
// auxilliary definitions
//-----------------------

struct UserSignInPresenterFixture
{
	MockCredentialsModel credentialsModel;
	MockEnService        enService;
	MockNoteListView     noteListView;
	MockUserModel        userModel;

	UserSignInPresenter signInPresenter;

	UserSignInPresenterFixture()
		: signInPresenter
			( credentialsModel
			, enService
			, noteListView
			, userModel
			)
	{
	}
};

//-----------
// test cases
//-----------

BOOST_FIXTURE_TEST_CASE
	( UserSignInPresenter_CredentialsSet
	, UserSignInPresenterFixture
	)
{
	SignalCheck signalCommitCheck;
	credentialsModel.ConnectCommit(ref(signalCommitCheck));

	credentialsModel.status = L"status";

	credentialsModel.SignalSet();

	BOOST_CHECK(!signalCommitCheck);
	BOOST_CHECK_EQUAL(credentialsModel.status, L"");

	credentialsModel.username = L"[anonymous]";
	signalCommitCheck.Reset();

	credentialsModel.SignalSet();

	BOOST_CHECK(signalCommitCheck);
	BOOST_CHECK_EQUAL(userModel.loadedAs, L"[anonymous]");
	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodLoadOrCreate);

	credentialsModel.username = L"username";
	signalCommitCheck.Reset();

	credentialsModel.SignalSet();

	BOOST_CHECK(!signalCommitCheck);
	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodNone);
	BOOST_CHECK_EQUAL(credentialsModel.status, L"Please, enter a password.");

	credentialsModel.password = L"password";
	userModel.validUsernames.insert(L"username");
	signalCommitCheck.Reset();

	credentialsModel.SignalSet();

	BOOST_CHECK(!signalCommitCheck);
	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodNone);
	BOOST_CHECK_EQUAL(credentialsModel.status, L"The password is invalid.");

	userModel.password = HashPassword(L"password");
	signalCommitCheck.Reset();

	credentialsModel.SignalSet();

	BOOST_CHECK(signalCommitCheck);
	BOOST_CHECK_EQUAL(userModel.loadMethod, MockUserModel::LoadMethodLoad);
}

BOOST_FIXTURE_TEST_CASE
	( UserSignInPresenter_SignIn
	, UserSignInPresenterFixture
	)
{
	userModel.username = L"username";

	noteListView.SignalSignIn();

	BOOST_CHECK_EQUAL(credentialsModel.username, L"[anonymous]");

	userModel.username = L"[anonymous]";

	noteListView.SignalSignIn();

	BOOST_CHECK_EQUAL(credentialsModel.username, L"");
}
