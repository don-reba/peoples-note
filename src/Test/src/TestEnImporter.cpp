#include "stdafx.h"
#include "EnImporter.h"

#include "Note.h"

#include <fstream>

using namespace boost;
using namespace std;

BOOST_AUTO_TEST_CASE(EnImporter_Test)
{
	EnImporter enImporter;

	wfstream file;
	file.open(L"data\\Mixed.enex");
	BOOST_REQUIRE(file.is_open());

	NoteBodyList bodies;
	NoteList notes;
	enImporter.ImportNotes(file, notes, bodies);

	BOOST_CHECK_EQUAL(notes.size(), 1);
	BOOST_CHECK_EQUAL(notes.at(0).GetTitle(), L"Mixed note");

	BOOST_CHECK_EQUAL(bodies.size(), 1);
	// TOOD: use a test case with shorter note body
}