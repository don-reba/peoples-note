#include "stdafx.h"
#include "NoteListModel.h"

#include "MockRegistryKey.h"
#include "MockUserModel.h"
#include "SignalCheck.h"

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

class ExceptionHasText
{
private:

	const char * const text;

public:

	ExceptionHasText(const char * text)
		: text (text)
	{
	}

	bool operator () (const std::exception & e)
	{
		return strcmp(e.what(), text) == 0;
	}
};

//-----------
// test cases
//-----------

BOOST_AUTO_TEST_CASE(NoteListModel_Empty)
{
	MockRegistryKey registryKey;

	MockUserModel userModel;
	NoteListModel noteListModel(2, userModel, registryKey);

	NoteList notes;
	bool hasPreviousPage, hasNextPage;

	noteListModel.GetCurrentPage(notes, hasPreviousPage, hasNextPage);

	BOOST_CHECK(notes.empty());

	BOOST_CHECK(!hasPreviousPage);
	BOOST_CHECK(!hasNextPage);

	BOOST_CHECK_EXCEPTION
		( noteListModel.SelectPreviousPage()
		, std::exception
		, ExceptionHasText("Invalid call to NoteListModel::SelectPreviousPage.")
		);
}

BOOST_AUTO_TEST_CASE(NoteListModel_Even)
{
	MockRegistryKey registryKey;

	MockUserModel userModel;
	NoteListModel noteListModel(3, userModel, registryKey);

	NoteList notes;
	bool hasNextPage, hasPreviousPage;

	userModel.notes.resize(6);

	noteListModel.GetCurrentPage(notes, hasPreviousPage, hasNextPage);

	BOOST_CHECK_EQUAL(notes.size(), 3);

	BOOST_CHECK(!hasPreviousPage);
	BOOST_CHECK(hasNextPage);

	noteListModel.SelectNextPage();

	noteListModel.GetCurrentPage(notes, hasPreviousPage, hasNextPage);
	BOOST_CHECK_EQUAL(notes.size(), 3);

	BOOST_CHECK(hasPreviousPage);
	BOOST_CHECK(!hasNextPage);
}

BOOST_AUTO_TEST_CASE(NoteListModel_Odd)
{
	MockRegistryKey registryKey;

	MockUserModel userModel;
	NoteListModel noteListModel(3, userModel, registryKey);

	NoteList notes;
	bool hasNextPage, hasPreviousPage;

	userModel.notes.resize(5);

	noteListModel.GetCurrentPage(notes, hasPreviousPage, hasNextPage);

	BOOST_CHECK_EQUAL(notes.size(), 3);

	BOOST_CHECK(!hasPreviousPage);
	BOOST_CHECK(hasNextPage);

	noteListModel.SelectNextPage();

	noteListModel.GetCurrentPage(notes, hasPreviousPage, hasNextPage);

	BOOST_CHECK_EQUAL(notes.size(), 2);

	BOOST_CHECK(hasPreviousPage);
	BOOST_CHECK(!hasNextPage);
}

BOOST_AUTO_TEST_CASE(NoteListModel_Reload)
{
	MockRegistryKey registryKey;

	MockUserModel userModel;
	NoteListModel noteListModel(2, userModel, registryKey);

	userModel.notes.resize(2);
	userModel.notes.at(0).name = L"note 0";
	userModel.notes.at(1).name = L"note 1";

	noteListModel.SelectNextPage();

	NoteList notes;
	bool hasPreviousPage, hasNextPage;

	noteListModel.GetCurrentPage(notes, hasPreviousPage, hasNextPage);

	BOOST_CHECK_EQUAL(notes.size(), 0);

	SignalCheck signalChangedCheck;
	noteListModel.ConnectChanged(ref(signalChangedCheck));

	noteListModel.Reload();

	noteListModel.GetCurrentPage(notes, hasPreviousPage, hasNextPage);
	BOOST_REQUIRE_EQUAL(notes.size(), 2);
	BOOST_CHECK_EQUAL(notes.at(0).name, L"note 0");
	BOOST_CHECK_EQUAL(notes.at(1).name, L"note 1");

	BOOST_CHECK(signalChangedCheck);
}

BOOST_AUTO_TEST_CASE(NoteListModel_NotebookTitle)
{
	MockRegistryKey registryKey;

	MockUserModel userModel;
	NoteListModel noteListModel(2, userModel, registryKey);

	noteListModel.SetNotebookTitleState(true);
	BOOST_CHECK_EQUAL(registryKey.GetString(L"notebook title state", L""), L"enabled");
	BOOST_CHECK(noteListModel.GetNotebookTitleState());

	noteListModel.SetNotebookTitleState(false);
	BOOST_CHECK_EQUAL(registryKey.GetString(L"notebook title state", L""), L"disabled");
	BOOST_CHECK(!noteListModel.GetNotebookTitleState());
}
