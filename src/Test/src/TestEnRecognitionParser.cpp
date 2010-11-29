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

	BOOST_CHECK_EQUAL(entries.at(0).Text,     L"CLIPS");
	BOOST_CHECK_EQUAL(entries.at(0).Weight,   68);
	BOOST_CHECK_EQUAL(entries.at(0).X,        151);
	BOOST_CHECK_EQUAL(entries.at(0).Y,        219);
	BOOST_CHECK_EQUAL(entries.at(0).W,        323);
	BOOST_CHECK_EQUAL(entries.at(0).H,        159);
	BOOST_CHECK_EQUAL(entries.at(0).Resource, Guid("{0}"));

	BOOST_CHECK_EQUAL(entries.at(1).Text,     L"NOTES");
	BOOST_CHECK_EQUAL(entries.at(1).Weight,   40);
	BOOST_CHECK_EQUAL(entries.at(1).X,        481);
	BOOST_CHECK_EQUAL(entries.at(1).Y,        387);
	BOOST_CHECK_EQUAL(entries.at(1).W,        266);
	BOOST_CHECK_EQUAL(entries.at(1).H,        120);
	BOOST_CHECK_EQUAL(entries.at(1).Resource, Guid("{0}"));

	BOOST_CHECK_EQUAL(entries.at(2).Text,     L"NOTTS");
	BOOST_CHECK_EQUAL(entries.at(2).Weight,   32);
	BOOST_CHECK_EQUAL(entries.at(2).X,        481);
	BOOST_CHECK_EQUAL(entries.at(2).Y,        387);
	BOOST_CHECK_EQUAL(entries.at(2).W,        266);
	BOOST_CHECK_EQUAL(entries.at(2).H,        120);
	BOOST_CHECK_EQUAL(entries.at(2).Resource, Guid("{0}"));
}

BOOST_AUTO_TEST_CASE(EnRecognitionParser_ParseEmpty)
{
	RecognitionEntryList entries;

	EnRecognitionParser parser;
	parser.Parse(L"", entries, Guid("{0}"));

	BOOST_REQUIRE_EQUAL(entries.size(), 0);
}