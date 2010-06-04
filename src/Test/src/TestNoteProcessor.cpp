#include "stdafx.h"
#include "NoteProcessor.h"

#include "MockNoteStore.h"
#include "MockUserModel.h"
#include "Notebook.h"

#include <algorithm>

using namespace std;

struct NoteProcessorFixture
{
	MockUserModel userModel;
	MockNoteStore noteStore;
	Notebook      notebook;

	NoteProcessor noteProcessor;

	NoteProcessorFixture()
		: noteProcessor (userModel, noteStore, notebook)
	{
	}
};

BOOST_FIXTURE_TEST_CASE(NoteProcessor_Add_Test, NoteProcessorFixture)
{
	notebook.name = L"test-notebook";

	EnInteropNote note;
	note.note.guid = Guid("{0}");
	note.note.name = L"test-note";
	note.resources.push_back(Guid("{1}"));
	note.resources.push_back(Guid("{2}"));

	noteStore.noteBodies["{0}"] = L"test-body";
	noteStore.resources.resize(2);
	noteStore.resources.at(0).Data.push_back(2);
	noteStore.resources.at(0).Guid = Guid("{1}");
	noteStore.resources.at(0).Hash = "9e688c58a5487b8eaf69c9e1005ad0bf";
	noteStore.resources.at(0).Note = Guid("{0}");
	noteStore.resources.at(1).Data.push_back(3);
	noteStore.resources.at(1).Data.push_back(5);
	noteStore.resources.at(1).Guid = Guid("{2}");
	noteStore.resources.at(1).Hash = "4816b20bbb2673692f5d8327d331fc00";
	noteStore.resources.at(1).Note = Guid("{0}");
	noteStore.noteTags.push_back(make_pair("{0}", L"tag-0"));
	noteStore.noteTags.push_back(make_pair("{0}", L"tag-1"));

	userModel.tags.push_back(Tag());
	userModel.tags.back().guid = Guid("{0}");
	userModel.tags.back().name = L"tag-0";
	userModel.tags.push_back(Tag());
	userModel.tags.back().guid = Guid("{1}");
	userModel.tags.back().name = L"tag-1";

	noteProcessor.Add(note);

	BOOST_CHECK_EQUAL(userModel.addedNotes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.addedNotes.at(0).body, L"test-body");
	BOOST_CHECK_EQUAL(userModel.addedNotes.at(0).note.name, L"test-note");

	BOOST_CHECK_EQUAL(userModel.resources.size(), 2);
	BOOST_CHECK_EQUAL
		( static_cast<string>(userModel.resources.at(0).Note)
		, "{0}"
		);
	BOOST_CHECK_EQUAL
		( static_cast<string>(userModel.resources.at(1).Note)
		, "{0}"
		);
	BOOST_CHECK_EQUAL
		( userModel.resources.at(0).Hash
		, "9e688c58a5487b8eaf69c9e1005ad0bf"
		);
	BOOST_CHECK_EQUAL
		( userModel.resources.at(1).Hash
		, "4816b20bbb2673692f5d8327d331fc00"
		);
	BOOST_CHECK_EQUAL(userModel.noteTags.size(), 2);
	BOOST_CHECK
		(  userModel.noteTags.find(MockUserModel::NoteTag("{0}", "{0}"))
		!= userModel.noteTags.end()
		);
	BOOST_CHECK
		(  userModel.noteTags.find(MockUserModel::NoteTag("{0}", "{1}"))
		!= userModel.noteTags.end()
		);
}

BOOST_FIXTURE_TEST_CASE(NoteProcessor_Delete_Test, NoteProcessorFixture)
{
	EnInteropNote note;
	note.note.name = L"test-note";

	noteProcessor.Delete(note);

	BOOST_CHECK_EQUAL(userModel.deletedNotes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.deletedNotes.at(0).name, L"test-note");
}

BOOST_FIXTURE_TEST_CASE(NoteProcessor_Merge_Test, NoteProcessorFixture)
{
	EnInteropNote local;
	local.note.name = local.name = L"local-note";

	EnInteropNote remote;
	remote.guid = remote.note.guid;
	remote.note.name = remote.name = L"remote-note";
	remote.resources.push_back(Guid("{0}"));
	remote.resources.push_back(Guid("{1}"));

	noteStore.noteBodies[remote.guid] = L"remote-body";
	noteStore.resources.push_back(Resource());
	noteStore.resources.back().Guid = Guid("{0}");
	noteStore.resources.back().Note = remote.guid;
	noteStore.resources.push_back(Resource());
	noteStore.resources.back().Guid = Guid("{1}");
	noteStore.resources.back().Note = remote.guid;

	noteProcessor.Merge(local, remote);

	BOOST_CHECK_EQUAL(userModel.notes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.notes.at(0).name, L"remote-note");

	BOOST_CHECK_EQUAL(userModel.resources.size(), 2);
	BOOST_CHECK_EQUAL(userModel.resources.at(0).Guid, Guid("{0}"));
	BOOST_CHECK_EQUAL(userModel.resources.at(1).Guid, Guid("{1}"));

	BOOST_CHECK_EQUAL(userModel.noteBodies[remote.guid], L"remote-body");
}

BOOST_FIXTURE_TEST_CASE(NoteProcessor_Upload_Test, NoteProcessorFixture)
{
	notebook.name = L"test-notebook";

	EnInteropNote note;
	note.note.name = L"test-note";
	note.note.guid = Guid("{0}");
	note.resources.push_back(Guid("{1}"));
	note.resources.push_back(Guid("{2}"));

	userModel.resources.resize(2);
	userModel.resources.at(0).Guid = Guid("{1}");
	userModel.resources.at(0).Hash = "1";
	userModel.resources.at(1).Guid = Guid("{2}");
	userModel.resources.at(1).Hash = "2";

	noteProcessor.Upload(note);

	BOOST_CHECK_EQUAL(noteStore.createdNotes.size(), 1);
	BOOST_CHECK_EQUAL(noteStore.createdNotes.at(0).name, L"test-note");

	BOOST_CHECK_EQUAL(noteStore.createdResources.size(), 2);
	BOOST_CHECK_EQUAL(noteStore.createdResources.at(0).Hash, "1");
	BOOST_CHECK_EQUAL(noteStore.createdResources.at(1).Hash, "2");
}
