#include "stdafx.h"
#include "NoteTagListPresenter.h"

#include "MockNoteTagListModel.h"
#include "MockNoteTagListView.h"
#include "MockNoteView.h"
#include "MockUserModel.h"

using namespace boost;
using namespace std;

//-----------------------
// auxilliary definitions
//-----------------------

struct NoteTagListPresenterFixture
{
	MockNoteTagListModel noteTagListModel;
	MockNoteTagListView  noteTagListView;
	MockNoteView         noteView;
	MockUserModel        userModel;

	NoteTagListPresenter noteTagListPresenter;

	NoteTagListPresenterFixture()
		: noteTagListPresenter
			( noteTagListModel
			, noteTagListView
			, noteView
			, userModel
			)
	{
	}
};

//-----------
// test cases
//-----------

BOOST_FIXTURE_TEST_CASE
	( NoteTagListPresenter_Cancel
	, NoteTagListPresenterFixture
	)
{
	noteTagListView.isShown = true;

	noteTagListView.SignalCancel();

	BOOST_CHECK(!noteTagListView.isShown);
}

BOOST_FIXTURE_TEST_CASE
	( NoteTagListPresenter_EditTags
	, NoteTagListPresenterFixture
	)
{
	noteTagListView.isShown = false;

	Note note;
	noteView.note = note;

	TagList tags(2);
	foreach (const Tag & tag, tags)
	{
		userModel.tags.push_back(tag);
		userModel.noteTags.insert(make_pair(note.guid, tag.guid));
	}

	noteView.SignalEditTags();

	BOOST_CHECK(noteTagListView.isShown);

	BOOST_CHECK_EQUAL(noteTagListModel.cleared.size(), 2);
	BOOST_CHECK_EQUAL(noteTagListModel.cleared.at(0).guid, tags.at(0).guid);
	BOOST_CHECK_EQUAL(noteTagListModel.cleared.at(1).guid, tags.at(1).guid);
}

BOOST_FIXTURE_TEST_CASE
	( NoteTagListPresenter_Ok
	, NoteTagListPresenterFixture
	)
{
	Note note;
	noteView.note = note;

	TagList tags(2);
	tags[0].name = L"0";
	tags[1].name = L"1";
	foreach (const Tag & tag, tags)
	{
		userModel.tags.push_back(tag);
		userModel.noteTags.insert(make_pair(note.guid, tag.guid));
	}

	// clean note

	noteTagListModel.isDirty = false;
	noteTagListView.isShown  = true;

	noteTagListView.SignalOk();

	BOOST_CHECK(!noteTagListView.isShown);
	BOOST_CHECK_EQUAL(userModel.noteTags.size(), 2);

	// dirty note

	noteTagListModel.selected.push_back(tags[1]);

	noteTagListModel.isDirty = true;

	noteTagListView.SignalOk();

	BOOST_CHECK_EQUAL(userModel.noteTags.size(), 1);
	BOOST_CHECK_EQUAL
		( userModel.noteTags.lower_bound(note.guid)->second
		, tags[1].guid
		);
}

BOOST_FIXTURE_TEST_CASE
	( NoteTagListPresenter_TagCleared
	, NoteTagListPresenterFixture
	)
{
	noteTagListView.SignalTagCleared();
	BOOST_CHECK_EQUAL
		( noteTagListModel.clearedTag
		, noteTagListView.activeTag
		);
}

BOOST_FIXTURE_TEST_CASE
	( NoteTagListPresenter_TagsChanged
	, NoteTagListPresenterFixture
	)
{
	noteTagListModel.cleared  = TagList(2);
	noteTagListModel.selected = TagList(3);

	noteTagListModel.SignalTagsChanged();

	BOOST_CHECK_EQUAL(noteTagListView.clearedTags.size(),  2);
	BOOST_CHECK_EQUAL(noteTagListView.selectedTags.size(), 3);
}

BOOST_FIXTURE_TEST_CASE
	( NoteTagListPresenter_TagSelected
	, NoteTagListPresenterFixture
	)
{
	noteTagListView.SignalTagSelected();
	BOOST_CHECK_EQUAL
		( noteTagListModel.selectedTag
		, noteTagListView.activeTag
		);
}
