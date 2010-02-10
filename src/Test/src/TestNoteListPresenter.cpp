#include "stdafx.h"
#include "MockNoteListModel.h"
#include "MockNoteListView.h"
#include "NoteListPresenter.h"
#include "MockNote.h"
#include "MockTag.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(NoteListPresenter_NoteListChanged_Test)
{
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockUserModel     userModel;
	NoteListPresenter presenter
		( noteListModel
		, noteListView
		, userModel
		);

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

	foreach (MockNote & note, notes)
		noteListModel.notes.push_back(&note);
	noteListModel.Reset();

	BOOST_CHECK_EQUAL(noteListView.notesUpdated, true);

	BOOST_CHECK_EQUAL(noteListView.notes.size(), 3);
	BOOST_CHECK_EQUAL
		( noteListView.notes[0]
		, L"<option class=\"note\"><table><tr><td rowspan=\"3\"><div id=\"thumb\" /></td><td>Note</td></tr><tr><td>tag-1, tag-2</td></tr><tr><td>2010-02-04 15:20</td></tr></table></option>"
		);
	BOOST_CHECK_EQUAL
		( noteListView.notes[1]
		, L"<option class=\"note\"><table><tr><td rowspan=\"3\"><div id=\"thumb\" /></td><td></td></tr><tr><td></td></tr><tr><td></td></tr></table></option>"
		);
	BOOST_CHECK_EQUAL
		( noteListView.notes[2]
		, L"<option class=\"note\"><table><tr><td rowspan=\"3\"><div id=\"thumb\" /></td><td>&lt;td id=&quot;</td></tr><tr><td>&amp;amp;</td></tr><tr><td>&lt;strong&gt;not bold&lt;/strong&gt;</td></tr></table></option>"
		);
}

BOOST_AUTO_TEST_CASE(NoteListPresenter_LoadLastUsedNotebook_Test)
{
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockUserModel     userModel;
	NoteListPresenter presenter
		( noteListModel
		, noteListView
		, userModel
		);

	vector<MockNote> notes(2);
	notes[0].title = L"note-0";
	notes[1].title = L"note-1";

	foreach (MockNote & note, notes)
		userModel.lastUsedNotebook.notes.push_back(&note);
	userModel.Loaded();

	BOOST_CHECK_EQUAL(noteListModel.notes.size(), 2);
	BOOST_CHECK_EQUAL(noteListModel.notes[0]->GetTitle(), L"note-0");
	BOOST_CHECK_EQUAL(noteListModel.notes[1]->GetTitle(), L"note-1");
}

BOOST_AUTO_TEST_CASE(NoteListPresenter_UpdateNotebookList_Test)
{
	MockNoteListModel noteListModel;
	MockNoteListView  noteListView;
	MockUserModel     userModel;
	NoteListPresenter presenter
		( noteListModel
		, noteListView
		, userModel
		);

	vector<MockNotebook> notebooks(2);
	notebooks[0].name = L"notebook-0";
	notebooks[1].name = L"notebook-1";

	foreach (MockNotebook & notebook, notebooks)
		userModel.notebooks.push_back(&notebook);
	userModel.Loaded();

	BOOST_CHECK_EQUAL(noteListView.notebooks.size(), 2);
	BOOST_CHECK_EQUAL(noteListView.notebooks[0], L"notebook-0");
	BOOST_CHECK_EQUAL(noteListView.notebooks[1], L"notebook-1");
}
