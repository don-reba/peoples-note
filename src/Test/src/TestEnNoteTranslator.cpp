#include "stdafx.h"

#include "EnNoteTranslator.h"
#include "MockUserModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "NotePresenter.h"

#include <iostream>

using namespace std;

BOOST_AUTO_TEST_CASE(EnNoteTranslator_ConvertToText)
{
	EnNoteTranslator enNoteTranslator;

	wstring xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note>"
			L"text1"
			L"<table><tr><td>cell1</td></tr><tr><td>cell2</td></tr></table>"
			L"text2"
			L"<en-crypt>CIPHER</en-crypt>"
			L"text3"
		L"</en-note>";
	wstring text;
	enNoteTranslator.ConvertToText(xml, text);

	BOOST_CHECK_EQUAL(text, L"text1 cell1 cell2 text2 text3 ");
}

BOOST_AUTO_TEST_CASE(EnNoteTranslator_ConvertToXml)
{
	EnNoteTranslator enNoteTranslator;

	wstring html;
	wstring xml;
	
	html = L"";
	enNoteTranslator.ConvertToXml(html, xml);
	BOOST_CHECK_EQUAL
		( xml
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note/>"
		);
	
	html = L"<div/>";
	enNoteTranslator.ConvertToXml(html, xml);
	BOOST_CHECK_EQUAL
		( xml
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note><div/></en-note>"
		);

	html = L"<p>1</p><p>2</p>";
	enNoteTranslator.ConvertToXml(html, xml);
	BOOST_CHECK_EQUAL
		( xml
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note><p>1</p><p>2</p></en-note>"
		);
	
	html = L"<en-note/>";
	enNoteTranslator.ConvertToXml(html, xml);
	BOOST_CHECK_EQUAL
		( xml
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note/>"
		);
	
	html = L"<en-note><p>1</p><p>2</p></en-note>";
	enNoteTranslator.ConvertToXml(html, xml);
	BOOST_CHECK_EQUAL
		( xml
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note><p>1</p><p>2</p></en-note>"
		);
}

BOOST_AUTO_TEST_CASE(EnNoteTranslator_Encrypt_Test)
{
	EnNoteTranslator enNoteTranslator;

	wstring xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note>"
			L"note"
			L"<en-crypt hint=\"Hint\">CIPHER</en-crypt>"
		L"</en-note>";
	wstring html;
	enNoteTranslator.ConvertToHtml(xml, html);

	BOOST_CHECK_EQUAL
		( html
		,
			L"<div type=\"en-note\">"
				L"note"
				L"<img hint=\"Hint\" type=\"en-crypt\" src=\"encrypt.png\" content=\"CIPHER\"/>"
			L"</div>"
		);

	wstring xml2;
	enNoteTranslator.ConvertToXml(html, xml2);
	BOOST_CHECK_EQUAL(xml, xml2);
}

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
