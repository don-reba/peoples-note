#include "stdafx.h"
#include "NoteTagListModel.h"

using namespace boost;
using namespace std;

Tag MakeTag(const wchar_t * name)
{
	Tag tag;
	tag.name = name;
	return tag;
}

BOOST_AUTO_TEST_CASE(TestNoteTagListModel_ClearTag)
{
	TagList cleared;
	cleared.push_back(MakeTag(L"a"));
	cleared.push_back(MakeTag(L"c"));

	TagList selected;
	selected.push_back(MakeTag(L"b"));

	NoteTagListModel model;

	BOOST_CHECK(!model.IsDirty());

	model.SetTags(cleared, selected);
	model.ClearTag(L"b");

	BOOST_CHECK(model.IsDirty());

	BOOST_CHECK_EQUAL(model.GetClearedTags().size(), 3);
	BOOST_CHECK_EQUAL(model.GetClearedTags().at(0).name, L"a");
	BOOST_CHECK_EQUAL(model.GetClearedTags().at(1).name, L"b");
	BOOST_CHECK_EQUAL(model.GetClearedTags().at(2).name, L"c");

	BOOST_CHECK_EQUAL(model.GetSelectedTags().size(), 0);
}

BOOST_AUTO_TEST_CASE(TestNoteTagListModel_SelectTag)
{
	TagList cleared;
	cleared.push_back(MakeTag(L"b"));

	TagList selected;
	selected.push_back(MakeTag(L"a"));
	selected.push_back(MakeTag(L"c"));

	NoteTagListModel model;

	BOOST_CHECK(!model.IsDirty());

	model.SetTags(cleared, selected);
	model.SelectTag(L"b");

	BOOST_CHECK(model.IsDirty());

	BOOST_CHECK_EQUAL(model.GetClearedTags().size(), 0);

	BOOST_CHECK_EQUAL(model.GetSelectedTags().size(), 3);
	BOOST_CHECK_EQUAL(model.GetSelectedTags().at(0).name, L"a");
	BOOST_CHECK_EQUAL(model.GetSelectedTags().at(1).name, L"b");
	BOOST_CHECK_EQUAL(model.GetSelectedTags().at(2).name, L"c");
}

BOOST_AUTO_TEST_CASE(TestNoteTagListModel_SetTags)
{
	TagList cleared;
	cleared.push_back(MakeTag(L"b"));

	TagList selected;
	selected.push_back(MakeTag(L"a"));
	selected.push_back(MakeTag(L"c"));

	NoteTagListModel model;

	BOOST_CHECK(!model.IsDirty());

	model.SetTags(cleared, selected);

	BOOST_CHECK(!model.IsDirty());

	BOOST_CHECK_EQUAL(model.GetClearedTags().size(), 1);
	BOOST_CHECK_EQUAL(model.GetClearedTags().at(0).name, L"b");

	BOOST_CHECK_EQUAL(model.GetSelectedTags().size(), 2);
	BOOST_CHECK_EQUAL(model.GetSelectedTags().at(0).name, L"a");
	BOOST_CHECK_EQUAL(model.GetSelectedTags().at(1).name, L"c");
}
