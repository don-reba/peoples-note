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
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note>"
			L"note"
			L"<en-media border=\"1\" hash=\"d978\" type=\"image/jpeg\"/>"
		L"</en-note>";
	wstring html;
	enNoteTranslator.ConvertToHtml(xml, html);

	BOOST_CHECK_EQUAL
		( html
		,
			L"<div type=\"en-note\">"
				L"note"
				L"<img border=\"1\" src=\"img:d978.jpg\"/>"
			L"</div>"
		);

	wstring xml2;
	enNoteTranslator.ConvertToXml(html, xml2);
	BOOST_CHECK_EQUAL(xml, xml2);
}

BOOST_AUTO_TEST_CASE(EnNoteTranslator_Todo_Test)
{
	EnNoteTranslator enNoteTranslator;

	wstring xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note>"
			L"note"
			L"<en-todo checked=\"true\"/>"
			L"<en-todo checked=\"false\"/>"
			L"<en-todo/>"
		L"</en-note>";
	wstring html;
	enNoteTranslator.ConvertToHtml(xml, html);

	BOOST_CHECK_EQUAL
		( html
		,
			L"<div type=\"en-note\">"
				L"note"
				L"<input type=\"checkbox\" checked=\"true\"/>"
				L"<input type=\"checkbox\"/>"
				L"<input type=\"checkbox\"/>"
			L"</div>"
		);


	wstring xml2;
	enNoteTranslator.ConvertToXml(html, xml2);
	BOOST_CHECK_EQUAL
		( xml2
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note>"
				L"note"
				L"<en-todo checked=\"true\"/>"
				L"<en-todo checked=\"false\"/>"
				L"<en-todo checked=\"false\"/>"
			L"</en-note>"
		);
}
