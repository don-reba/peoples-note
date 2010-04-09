#include "stdafx.h"
#include "MockNoteListView.h"
#include "MockUserModel.h"
#include "UserSignInPresenter.h"

BOOST_AUTO_TEST_CASE(UserSignInPresenter_Upate_Test)
{
	MockNoteListView noteListView;
	MockUserModel    userModel;

	UserSignInPresenter(noteListView, userModel);

	userModel.credentialsModel.SignalUpdated();
	BOOST_CHECK(noteListView.signedIn);
}

BOOST_AUTO_TEST_CASE(UserSignInPresenter_SignIn_Test)
{
	MockNoteListView noteListView;
	MockUserModel    userModel;

	UserSignInPresenter(noteListView, userModel);

	userModel.credentialsModel.updated = true;
	noteListView.SignalSignIn();
	BOOST_CHECK(!userModel.credentialsModel.updated);
}
