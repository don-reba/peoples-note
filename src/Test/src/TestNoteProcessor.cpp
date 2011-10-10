#include "stdafx.h"
#include "NoteProcessor.h"

#include "EnNoteTranslator.h"
#include "MockNoteStore.h"
#include "MockUserModel.h"
#include "Notebook.h"

#include <algorithm>

using namespace std;

struct NoteProcessorFixture
{
	EnNoteTranslator enNoteTranslator;
	MockUserModel    userModel;
	MockNoteStore    noteStore;
	Notebook         notebook;

	NoteProcessor noteProcessor;

	NoteProcessorFixture()
		: noteProcessor
			( enNoteTranslator
			, userModel
			, noteStore
			, notebook
			)
	{
	}
};

BOOST_FIXTURE_TEST_CASE(NoteProcessor_AddLocal_Test, NoteProcessorFixture)
{
	notebook.name = L"test-notebook";

	EnInteropNote note;
	note.note.guid = Guid("{0}");
	note.note.name = L"test-note";
	note.resources.push_back(Guid("{1}"));
	note.resources.push_back(Guid("{2}"));

	noteStore.noteBodies["{0}"] = L"<en-note>test-body</en-note>";
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
	userModel.tags.push_back(Tag());
	userModel.tags.back().guid = Guid("{2}");
	userModel.tags.back().name = L"tag-2";

	userModel.noteTags.insert(make_pair(Guid("{0}"), Guid("{2}")));

	noteProcessor.AddLocal(note);

	BOOST_REQUIRE_EQUAL(userModel.addedNotes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.addedNotes.at(0).body, L"<en-note>test-body</en-note>");
	BOOST_CHECK_EQUAL(userModel.addedNotes.at(0).note.name, L"test-note");

	BOOST_REQUIRE_EQUAL(userModel.resources.size(), 2);
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
}

BOOST_FIXTURE_TEST_CASE(NoteProcessor_CreateRemote_Test, NoteProcessorFixture)
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

	noteProcessor.CreateRemote(note);

	BOOST_CHECK_EQUAL(noteStore.createdNotes.size(), 1);
	BOOST_CHECK_EQUAL(noteStore.createdNotes.at(0).name, L"test-note");

	BOOST_CHECK_EQUAL(noteStore.createdResources.size(), 2);
	BOOST_CHECK_EQUAL(noteStore.createdResources.at(0).Hash, "1");
	BOOST_CHECK_EQUAL(noteStore.createdResources.at(1).Hash, "2");
}

BOOST_FIXTURE_TEST_CASE(NoteProcessor_DeleteLocal_Test, NoteProcessorFixture)
{
	EnInteropNote note;
	note.guid = note.note.guid;

	noteProcessor.DeleteLocal(note);

	BOOST_CHECK_EQUAL(userModel.expungedNotes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.expungedNotes.at(0), note.guid);
}

BOOST_FIXTURE_TEST_CASE(NoteProcessor_MergeLocal_Test, NoteProcessorFixture)
{
	EnInteropNote local;
	local.note.name = local.name = L"local-note";

	EnInteropNote remote;
	remote.guid = remote.note.guid;
	remote.note.name = remote.name = L"remote-note";
	remote.resources.push_back(Guid("{0}"));
	remote.resources.push_back(Guid("{1}"));

	noteStore.noteBodies[remote.guid] = L"<en-note>remote-body</en-note>";
	noteStore.resources.push_back(Resource());
	noteStore.resources.back().Guid = Guid("{0}");
	noteStore.resources.back().Note = remote.guid;
	noteStore.resources.push_back(Resource());
	noteStore.resources.back().Guid = Guid("{1}");
	noteStore.resources.back().Note = remote.guid;

	noteProcessor.MergeLocal(local, remote);

	BOOST_CHECK_EQUAL(userModel.notes.size(), 1);
	BOOST_CHECK_EQUAL(userModel.notes.at(0).name, L"remote-note");

	BOOST_CHECK_EQUAL(userModel.resources.size(), 2);
	BOOST_CHECK_EQUAL(userModel.resources.at(0).Guid, Guid("{0}"));
	BOOST_CHECK_EQUAL(userModel.resources.at(1).Guid, Guid("{1}"));

	BOOST_CHECK_EQUAL(userModel.noteBodies[remote.guid], L"<en-note>remote-body</en-note>");
}

//BOOST_FIXTURE_TEST_CASE(NoteProcessor_Update_Test, NoteProcessorFixture)
//{
//	notebook.name = L"test-notebook";
//
//	EnInteropNote note;
//	note.note.name = L"test-note";
//	note.note.guid = Guid("{0}");
//	note.resources.push_back(Guid("{1}"));
//	note.resources.push_back(Guid("{2}"));
//
//	userModel.resources.resize(2);
//	userModel.resources.at(0).Guid = Guid("{1}");
//	userModel.resources.at(0).Hash = "1";
//	userModel.resources.at(1).Guid = Guid("{2}");
//	userModel.resources.at(1).Hash = "2";
//
//	noteProcessor.Update(note);
//
//	BOOST_CHECK_EQUAL(noteStore.createdNotes.size(), 1);
//	BOOST_CHECK_EQUAL(noteStore.createdNotes.at(0).name, L"test-note");
//
//	BOOST_CHECK_EQUAL(noteStore.createdResources.size(), 2);
//	BOOST_CHECK_EQUAL(noteStore.createdResources.at(0).Hash, "1");
//	BOOST_CHECK_EQUAL(noteStore.createdResources.at(1).Hash, "2");
//}
