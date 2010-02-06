#include "stdafx.h"
#include "CredentialsModel.h"
#include "CredentialsPresenter.h"
#include "CurrentUserLoader.h"
#include "LastUserModel.h"
#include "MockAppModel.h"
#include "MockCredentialsModel.h"
#include "MockCredentialsView.h"
#include "MockLastUserModel.h"
#include "MockNote.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "MockRegistryKey.h"
#include "MockUserModel.h"
#include "NoteListPresenter.h"
#include "Tools.h"

#include <iostream>
#include <string>

#define BOOST_TEST_MODULE const_string test
#include <boost/test/unit_test.hpp>

using namespace boost;
using namespace std;

BOOST_TEST_DONT_PRINT_LOG_VALUE(std::wstring)

BOOST_AUTO_TEST_CASE(ToolsConvertToUnicode_Test)
{
	wstring testString = Tools::ConvertToUnicode("Test");
	BOOST_CHECK_EQUAL(testString, L"Test");

	wstring emptyString = Tools::ConvertToUnicode("");
	BOOST_CHECK_EQUAL(emptyString, L"");
}

BOOST_AUTO_TEST_CASE(ToolsConvertToAnsi_Test)
{
	string testString = Tools::ConvertToAnsi(L"Test");
	BOOST_CHECK_EQUAL(testString, "Test");

	string emptyString = Tools::ConvertToAnsi(L"");
	BOOST_CHECK_EQUAL(emptyString, "");
}

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

BOOST_AUTO_TEST_CASE(NoteListPresenter_Reset_Test)
{
	MockNoteListModel model;
	MockNoteListView  view;
	NoteListPresenter presenter(model, view);

	vector<MockTag> tags(3);
	tags[0].name = L"tag-1";
	tags[1].name = L"tag-2";
	tags[2].name = L"&amp;";

	vector<MockNote> notes(3);

	notes[0].title = L"Note";
	notes[0].tags.push_back(tags[0]);
	notes[0].tags.push_back(tags[1]);
	notes[0].createDate.formattedDateTime = L"2010-02-04 15:20";

	notes[1].title = L"";
	notes[1].createDate.formattedDateTime = L"";

	notes[2].title = L"<td id=\"";
	notes[2].tags.push_back(tags[2]);
	notes[2].createDate.formattedDateTime = L"<strong>not bold</strong>";

	foreach (const MockNote & note, notes)
		model.notes.push_back(note);
	model.Reset();

	BOOST_CHECK_EQUAL(view.updated, true);

	BOOST_CHECK_EQUAL(view.notes.size(), 3);
	BOOST_CHECK_EQUAL
		( Tools::ConvertToAnsi(view.notes[0])
		, Tools::ConvertToAnsi(L"<option class=\"note\"><table><tr><td rowspan=\"3\"><div id=\"thumb\" /></td><td>Note</td></tr><tr><td>tag-1, tag-2</td></tr><tr><td>2010-02-04 15:20</td></tr></table></option>")
		);
	BOOST_CHECK_EQUAL
		( Tools::ConvertToAnsi(view.notes[1])
		, Tools::ConvertToAnsi(L"<option class=\"note\"><table><tr><td rowspan=\"3\"><div id=\"thumb\" /></td><td></td></tr><tr><td></td></tr><tr><td></td></tr></table></option>")
		);
	BOOST_CHECK_EQUAL
		( Tools::ConvertToAnsi(view.notes[2])
		, Tools::ConvertToAnsi(L"<option class=\"note\"><table><tr><td rowspan=\"3\"><div id=\"thumb\" /></td><td>&lt;td id=&quot;</td></tr><tr><td>&amp;amp;</td></tr><tr><td>&lt;strong&gt;not bold&lt;/strong&gt;</td></tr></table></option>")
		);
}

BOOST_AUTO_TEST_CASE(CurrentUserLoader_DefaultUser_Test)
{
	MockAppModel      appModel;
	MockUserModel     userModel;
	MockLastUserModel lastUserModel;
	CurrentUserLoader(appModel, userModel, lastUserModel);

	appModel.Start();

	BOOST_CHECK(userModel.isDefault);
	BOOST_CHECK(userModel.isLoaded);
}

BOOST_AUTO_TEST_CASE(CurrentUserLoader_Test)
{
	MockAppModel      appModel;
	MockUserModel     userModel;
	MockLastUserModel lastUserModel;
	CurrentUserLoader(appModel, userModel, lastUserModel);

	lastUserModel.credentialsModel.username = L"test-usr";
	lastUserModel.credentialsModel.password = L"test-pwd";

	appModel.Start();

	BOOST_CHECK(!userModel.isDefault);
	BOOST_CHECK(userModel.isLoaded);

	BOOST_CHECK_EQUAL(userModel.credentialsModel.username, L"test-usr");
	BOOST_CHECK_EQUAL(userModel.credentialsModel.password, L"test-pwd");
}

BOOST_AUTO_TEST_CASE(CurrentUserLoader_NoPassword_Test)
{
	MockAppModel      appModel;
	MockUserModel     userModel;
	MockLastUserModel lastUserModel;
	CurrentUserLoader(appModel, userModel, lastUserModel);

	lastUserModel.credentialsModel.username = L"test-usr";
	lastUserModel.credentialsModel.password = L"";

	appModel.Start();

	BOOST_CHECK(!userModel.isDefault);
	BOOST_CHECK(userModel.isLoaded);

	BOOST_CHECK_EQUAL(userModel.credentialsModel.username, L"test-usr");
	BOOST_CHECK_EQUAL(userModel.credentialsModel.password, L"");
}

BOOST_AUTO_TEST_CASE(CredentialsModel_Test)
{
	CredentialsModel model;
	model.SetCredentials(L"test-usr", L"test-pwd");

	BOOST_CHECK_EQUAL(model.GetUsername(), L"test-usr");
	BOOST_CHECK_EQUAL(model.GetPassword(), L"test-pwd");
}

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
