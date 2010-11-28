#include "stdafx.h"
#include "EnRecognitionParser.h"

BOOST_AUTO_TEST_CASE(EnRecognitionParser_Parse)
{
	// partial example from: http://www.evernote.com/about/developer/api/evernote-api.htm
	const wchar_t * xml =
		L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		L"<!DOCTYPE recoIndex SYSTEM \"http://xml.evernote.com/pub/recoIndex.dtd\">"
		L"<recoIndex"
			L" docType=\"handwritten\""
			L" objType=\"image\""
			L" objID=\"47aa2ac0e29962f3699abe50f1afa996\""
			L" engineVersion=\"1.9.4.6\""
			L" recoType=\"client\""
			L" lang=\"en\""
			L" objWidth=\"1600\""
			L" objHeight=\"1200\""
			L">"
			L"<item x=\"151\" y=\"219\" w=\"323\" h=\"159\">"
				L"<t w=\"68\">CLIPS</t>"
			L"</item>"
			L"<item x=\"481\" y=\"387\" w=\"266\" h=\"120\">"
				L"<t w=\"40\">NOTES</t>"
				L"<t w=\"32\">NOTTS</t>"
			L"</item>"
		L"</recoIndex>";

	RecognitionEntryList entries;

	EnRecognitionParser parser;
	parser.Parse(xml, entries, Guid("{0}"));

	BOOST_REQUIRE_EQUAL(entries.size(), 3);

	BOOST_CHECK_EQUAL(entries.at(0).text,     L"CLIPS");
	BOOST_CHECK_EQUAL(entries.at(0).weight,   68);
	BOOST_CHECK_EQUAL(entries.at(0).x,        151);
	BOOST_CHECK_EQUAL(entries.at(0).y,        219);
	BOOST_CHECK_EQUAL(entries.at(0).w,        323);
	BOOST_CHECK_EQUAL(entries.at(0).h,        159);
	BOOST_CHECK_EQUAL(entries.at(0).resource, Guid("{0}"));

	BOOST_CHECK_EQUAL(entries.at(1).text,     L"NOTES");
	BOOST_CHECK_EQUAL(entries.at(1).weight,   40);
	BOOST_CHECK_EQUAL(entries.at(1).x,        481);
	BOOST_CHECK_EQUAL(entries.at(1).y,        387);
	BOOST_CHECK_EQUAL(entries.at(1).w,        266);
	BOOST_CHECK_EQUAL(entries.at(1).h,        120);
	BOOST_CHECK_EQUAL(entries.at(1).resource, Guid("{0}"));

	BOOST_CHECK_EQUAL(entries.at(2).text,     L"NOTTS");
	BOOST_CHECK_EQUAL(entries.at(2).weight,   32);
	BOOST_CHECK_EQUAL(entries.at(2).x,        481);
	BOOST_CHECK_EQUAL(entries.at(2).y,        387);
	BOOST_CHECK_EQUAL(entries.at(2).w,        266);
	BOOST_CHECK_EQUAL(entries.at(2).h,        120);
	BOOST_CHECK_EQUAL(entries.at(2).resource, Guid("{0}"));
}
