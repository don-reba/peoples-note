#include "stdafx.h"
#include "TagProcessor.h"

#include "MockNoteStore.h"
#include "MockUserModel.h"
#include "Tag.h"

#include <algorithm>

using namespace std;

BOOST_AUTO_TEST_CASE(TagProcessor_Add_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;

	TagProcessor noteProcessor
		( noteStore
		, userModel
		);

	Tag tag(L"test");

	noteProcessor.Add(tag);

	BOOST_CHECK_EQUAL(userModel.tags.size(), 1);
	BOOST_CHECK_EQUAL(userModel.tags.at(0).GetName(), L"test");
}

BOOST_AUTO_TEST_CASE(TagProcessor_Delete_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;

	TagProcessor noteProcessor
		( noteStore
		, userModel
		);

	Tag tag(L"test");

	noteProcessor.Delete(tag);

	BOOST_CHECK_EQUAL(userModel.deletedTags.size(), 1);
	BOOST_CHECK_EQUAL(userModel.deletedTags.at(0).GetName(), L"test");
}

BOOST_AUTO_TEST_CASE(TagProcessor_Rename_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;

	TagProcessor noteProcessor
		( noteStore
		, userModel
		);

	userModel.tags.push_back(Tag(L"test"));
	userModel.tags.push_back(Tag(L"test(1)"));
	userModel.tags.push_back(Tag(L"test(2)"));
	userModel.tags.push_back(Tag(L"test(3)"));

	Tag tag(L"test");

	noteProcessor.Rename(tag);

	BOOST_CHECK_EQUAL(userModel.tags.size(), 5);
	BOOST_CHECK_EQUAL(userModel.tags.at(4).GetName(), L"test(4)");
}

BOOST_AUTO_TEST_CASE(TagProcessor_Upload_Test)
{
	MockNoteStore noteStore;
	MockUserModel userModel;

	TagProcessor noteProcessor
		( noteStore
		, userModel
		);

	Tag tag(L"test");

	noteProcessor.Upload(tag);

	BOOST_CHECK_EQUAL(noteStore.createdTags.size(), 1);
	BOOST_CHECK_EQUAL(noteStore.createdTags.at(0).GetName(), L"test");
}
