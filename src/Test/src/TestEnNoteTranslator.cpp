#include "stdafx.h"

#include "EnNoteTranslator.h"
#include "MockUserModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "NotePresenter.h"

#include <iostream>

using namespace std;

BOOST_AUTO_TEST_CASE(EnNoteTranslator_InvalidChars)
{
	EnNoteTranslator enNoteTranslator;

	wstring html;
	wstring xml;
	
	html = L"<a href=\"http://www.google.com/search?q=TEST&amp;ie=UTF-8\">Google</a>";
	enNoteTranslator.ConvertToXml(html, xml);
	BOOST_CHECK_EQUAL
		( xml
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note><a href=\"http://www.google.com/search?q=TEST&amp;ie=UTF-8\">Google</a></en-note>"
		);

	enNoteTranslator.ConvertToHtml(xml, html);
	BOOST_CHECK_EQUAL
		( html
		, L"<a href=\"http://www.google.com/search?q=TEST&amp;ie=UTF-8\">Google</a>"
		);
}

BOOST_AUTO_TEST_CASE(EnNoteTranslator_ConvertToText)
{
	EnNoteTranslator enNoteTranslator;

	wstring xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note>"
			L"<p>text1</p>"
			L"<table><tr><td>cell1</td></tr><tr><td>cell2</td></tr></table>"
			L"<p>text2</p>"
			L"<en-crypt>CIPHER</en-crypt>"
			L"<p>text3.text4</p>"
			L"<p>&aelig;</p>"
		L"</en-note>";
	wstring text;
	enNoteTranslator.ConvertToText(xml, text);

	BOOST_CHECK_EQUAL(text, L"text1 cell1 cell2 text2 text3 text4         ");
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
	
	html = L"<p/><p></p>";
	enNoteTranslator.ConvertToXml(html, xml);
	BOOST_CHECK_EQUAL
		( xml
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note><p>&nbsp;</p><p>&nbsp;</p></en-note>"
		);
}

BOOST_AUTO_TEST_CASE(EnNoteTranslator_Encrypt_Test)
{
	EnNoteTranslator enNoteTranslator;

	wstring xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note>"
			L"<p>note</p>"
			L"<en-crypt hint=\"Hint\">CIPHER</en-crypt>"
		L"</en-note>";
	wstring html;
	enNoteTranslator.ConvertToHtml(xml, html);

	BOOST_CHECK_EQUAL
		( html
		,
			L"<p>note</p>"
			L"<img hint=\"Hint\" type=\"en-crypt\" src=\"encrypt.png\" content=\"CIPHER\"/>"
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
			L"<p>note</p>"
			L"<en-media border=\"1\" hash=\"d978\" type=\"image/jpeg\"/>"
		L"</en-note>";
	wstring html;
	enNoteTranslator.ConvertToHtml(xml, html);

	BOOST_CHECK_EQUAL
		( html
		,
			L"<p>note</p>"
			L"<img border=\"1\" src=\"img:d978.jpg\"/>"
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
			L"<p>note</p>"
			L"<en-todo checked=\"true\"/>"
			L"<en-todo checked=\"false\"/>"
			L"<en-todo/>"
		L"</en-note>";
	wstring html;
	enNoteTranslator.ConvertToHtml(xml, html);

	BOOST_CHECK_EQUAL
		( html
		,
			L"<p>note</p>"
			L"<input type=\"checkbox\" checked=\"true\"/>"
			L"<input type=\"checkbox\"/>"
			L"<input type=\"checkbox\"/>"
		);


	wstring xml2;
	enNoteTranslator.ConvertToXml(html, xml2);
	BOOST_CHECK_EQUAL
		( xml2
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note>"
				L"<p>note</p>"
				L"<en-todo checked=\"true\"/>"
				L"<en-todo checked=\"false\"/>"
				L"<en-todo checked=\"false\"/>"
			L"</en-note>"
		);
}
