#include "stdafx.h"

#include "EnNoteTranslator.h"
#include "MockUserModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "NotePresenter.h"

using namespace std;

BOOST_AUTO_TEST_CASE(EnNoteTranslator_Media_Test)
{
	EnNoteTranslator enNoteTranslator;

	wstring xml =
		L"<en-note>"
		L"note"
		L"<en-media border=\"1\" hash=\"d978\" type=\"image/jpeg\" />"
		L"</en-note>";
	wstring html;
	enNoteTranslator.ConvertToHtml(xml, html);

	BOOST_CHECK_EQUAL
		( html
		, L"<div>"
			L"note"
			L"<img src=\"img:d978\" border=\"1\"/>"
			L"</div>"
		);
}

BOOST_AUTO_TEST_CASE(EnNoteTranslator_Todo_Test)
{
	EnNoteTranslator enNoteTranslator;

	wstring xml =
		L"<en-note>"
		L"note"
		L"<en-todo checked=\"true\" />"
		L"<en-todo checked=\"false\" />"
		L"</en-note>";
	wstring html;
	enNoteTranslator.ConvertToHtml(xml, html);

	BOOST_CHECK_EQUAL
		( html
		, L"<div>"
			L"note"
			L"<input type=\"checkbox\" checked=\"\"/>"
			L"<input type=\"checkbox\"/>"
			L"</div>"
		);
}
