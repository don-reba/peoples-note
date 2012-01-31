#include "stdafx.h"
#include "EnImporter.h"

#include "Note.h"
#include "Tools.h"

#include <fstream>
#include <iterator>

using namespace boost;
using namespace std;
using namespace Tools;

//-----------
// test suite
//-----------

BOOST_AUTO_TEST_CASE(EnImporter_Test)
{
	EnImporter enImporter;

	wfstream stream;
	stream.open(L"data\\Mixed.enex");
	BOOST_REQUIRE(stream.is_open());
	wstring text
		( (istreambuf_iterator<wchar_t>(stream))
		, istreambuf_iterator<wchar_t>()
		);


	NoteList             notes;
	NoteBodyList         bodies;
	ResourceList         resources;
	RecognitionEntryList recognitionEntries;

	enImporter.ImportNotes
		( text
		, notes
		, bodies
		, resources
		, recognitionEntries
		);

	BOOST_REQUIRE_EQUAL(notes.size(), 1);
	BOOST_CHECK_EQUAL(notes.at(0).name, L"Mixed note");

	BOOST_REQUIRE_EQUAL(bodies.size(), 1);
	BOOST_CHECK(StartsWith(bodies.at(0), wstring(L"<?xml version")));
	BOOST_CHECK(EndsWith(bodies.at(0),   wstring(L"</en-note>\n")));

	BOOST_REQUIRE_EQUAL(resources.size(), 1);
	BYTE resourceStart[] = { 0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46 };
	BYTE resourceEnd[]   = { 0x76, 0xC4, 0x6B, 0x1D, 0x28, 0x87, 0xFF, 0xD9 };
	BOOST_CHECK(StartsWith (resources.at(0).Data, resourceStart, resourceStart + 8));
	BOOST_CHECK(EndsWith   (resources.at(0).Data, resourceEnd,   resourceEnd   + 8));

	BOOST_REQUIRE_EQUAL(recognitionEntries.size(), 9);
	BOOST_CHECK_EQUAL(recognitionEntries.at(0).Text, L"North");
	BOOST_CHECK_EQUAL(recognitionEntries.at(1).Text, L"Nortii");
	BOOST_CHECK_EQUAL(recognitionEntries.at(2).Text, L"East");
	BOOST_CHECK_EQUAL(recognitionEntries.at(3).Text, L"FAST");
	BOOST_CHECK_EQUAL(recognitionEntries.at(4).Text, L"Registry");
	BOOST_CHECK_EQUAL(recognitionEntries.at(5).Text, L"IM");
	BOOST_CHECK_EQUAL(recognitionEntries.at(6).Text, L"LTD");
	BOOST_CHECK_EQUAL(recognitionEntries.at(7).Text, L"LTD.");
	BOOST_CHECK_EQUAL(recognitionEntries.at(8).Text, L"ud.");
}
