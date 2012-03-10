#include "stdafx.h"
#include "EnNoteTranslator.h"

#include "MockUserModel.h"
#include "MockNoteListView.h"
#include "MockNoteView.h"
#include "NotePresenter.h"

#include <iostream>
#include <set>

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

	enNoteTranslator.ConvertToHtml(xml, html, true);
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

	html = L"<p><span>text</span></p>\n";
	enNoteTranslator.ConvertToXml(html, xml);
	BOOST_CHECK_EQUAL
		( xml
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note><p><span>text</span></p></en-note>"
		);
}

BOOST_AUTO_TEST_CASE(EnNoteTranslator_Encrypt)
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
	enNoteTranslator.ConvertToHtml(xml, html, true);

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

BOOST_AUTO_TEST_CASE(EnNoteTranslator_FontSize)
{
	EnNoteTranslator enNoteTranslator;

	wstring xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note>Hello <font size=\"1\" align=\"left\"><div size=\"2\">World</div></font>!</en-note>";
	wstring html;
	enNoteTranslator.ConvertToHtml(xml, html, true);

	BOOST_CHECK_EQUAL
		( html
		, L"Hello <font align=\"left\"><div size=\"2\">World</div></font>!"
		);
}

BOOST_AUTO_TEST_CASE(EnNoteTranslator_Media)
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
	enNoteTranslator.ConvertToHtml(xml, html, true);

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

BOOST_AUTO_TEST_CASE(EnNoteTranslator_Todo)
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
	enNoteTranslator.ConvertToHtml(xml, html, true);

	BOOST_CHECK_EQUAL
		( html
		,
			L"<p>note</p>"
			L"<input type=\"checkbox\" checked=\"true\" uid=\"0\"/>"
			L"<input type=\"checkbox\" uid=\"1\"/>"
			L"<input type=\"checkbox\" uid=\"2\"/>"
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

BOOST_AUTO_TEST_CASE(EnNoteTranslator_PhoneNumbers)
{
	EnNoteTranslator enNoteTranslator;

	wstring xml, html;

	// international format
	xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note><p>Hello +34 (123) 456-789 World!</p></en-note>";
	enNoteTranslator.ConvertToHtml(xml, html, true);
	BOOST_CHECK_EQUAL
		( html
		, L"<p>Hello <a href=\"tel:+34 (123) 456-789\">+34 (123) 456-789</a> World!</p>"
		);

	// continuous
	xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note><p>Hello +1234567 World!</p></en-note>";
	enNoteTranslator.ConvertToHtml(xml, html, true);
	BOOST_CHECK_EQUAL
		( html
		, L"<p>Hello <a href=\"tel:+1234567\">+1234567</a> World!</p>"
		);

	// too few digits
	xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note><p>Hello +1234 World!</p></en-note>";
	enNoteTranslator.ConvertToHtml(xml, html, true);
	BOOST_CHECK_EQUAL
		( html
		, L"<p>Hello +1234 World!</p>"
		);

	// russian; national
	xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note><p>Hello 8.812.234-56-78 World!</p></en-note>";
	enNoteTranslator.ConvertToHtml(xml, html, true);
	BOOST_CHECK_EQUAL
		( html
		, L"<p>Hello <a href=\"tel:8.812.234-56-78\">8.812.234-56-78</a> World!</p>"
		);

	// multiple instances; end of sentence
	xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note><p>12345, 12345.</p></en-note>";
	enNoteTranslator.ConvertToHtml(xml, html, true);
	BOOST_CHECK_EQUAL
		( html
		, L"<p><a href=\"tel:12345\">12345</a>, <a href=\"tel:12345\">12345</a>.</p>"
		);
}

BOOST_AUTO_TEST_CASE(EnNoteTranslator_XmlTranformation)
{
	EnNoteTranslator enNoteTranslator;

	wstring srcXml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
		L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
		L"<en-note>"
			L"<en-todo checked=\"true\"/>"
			L"<en-todo checked=\"false\"/>"
			L"<en-todo checked=\"false\"/>"
			L"<en-todo checked=\"true\"/>"
		L"</en-note>";
	set<int> dirtyCheckboxIds;
	dirtyCheckboxIds.insert(-1);
	dirtyCheckboxIds.insert(0);
	dirtyCheckboxIds.insert(2);
	dirtyCheckboxIds.insert(3);
	dirtyCheckboxIds.insert(4);

	wstring dstXml;
	enNoteTranslator.ApplyXmlModifications(srcXml, dstXml, dirtyCheckboxIds);

	BOOST_CHECK_EQUAL
		( dstXml
		,
			L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
			L"<!DOCTYPE en-note SYSTEM \"http://xml.evernote.com/pub/enml2.dtd\">\n"
			L"<en-note>"
				L"<en-todo checked=\"false\"/>"
				L"<en-todo checked=\"false\"/>"
				L"<en-todo checked=\"true\"/>"
				L"<en-todo checked=\"false\"/>"
			L"</en-note>"
		);
}
