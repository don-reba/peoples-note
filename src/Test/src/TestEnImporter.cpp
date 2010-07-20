#include "stdafx.h"
#include "EnImporter.h"

#include "Note.h"

#include <fstream>
#include <iterator>

using namespace boost;
using namespace std;

//----------------------
// auxillary definitions
//----------------------

template<typename T, typename I>
bool StartsWith(const T & sequence, I begin, I end)
{
	if (sequence.size() < static_cast<size_t>(end - begin))
		return false;
	T::const_iterator i = sequence.begin();
	while (begin != end)
	{
		if (*i != *begin)
			return false;
		++i; ++begin;
	}
	return true;
}

template<typename T>
bool StartsWith(const T & sequence, T & fragment)
{
	return StartsWith(sequence, fragment.begin(), fragment.end());
}

template<typename T, typename I>
bool EndsWith(const T & sequence, I begin, I end)
{
	if (sequence.size() < static_cast<size_t>(end - begin))
		return false;
	T::const_iterator i = sequence.end() - (end - begin);
	while (begin != end)
	{
		if (*i != *begin)
			return false;
		++i; ++begin;
	}
	return true;
}

template<typename T>
bool EndsWith(const T & sequence, const T & fragment)
{
	return EndsWith(sequence, fragment.begin(), fragment.end());
}

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

	NoteBodyList bodies;
	NoteList     notes;
	ResourceList resources;
	enImporter.ImportNotes(text, notes, bodies, resources);

	BOOST_CHECK_EQUAL(notes.size(), 1);
	BOOST_CHECK_EQUAL(notes.at(0).name, L"Mixed note");

	BOOST_CHECK_EQUAL(bodies.size(), 1);
	BOOST_CHECK(StartsWith(bodies.at(0), wstring(L"<?xml version")));
	BOOST_CHECK(EndsWith(bodies.at(0),   wstring(L"</en-note>\n")));

	BOOST_CHECK_EQUAL(resources.size(), 1);
	BYTE resourceStart[] = { 0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46 };
	BYTE resourceEnd[]   = { 0x76, 0xC4, 0x6B, 0x1D, 0x28, 0x87, 0xFF, 0xD9 };
	BOOST_CHECK(StartsWith (resources.at(0).Data, resourceStart, resourceStart + 8));
	BOOST_CHECK(EndsWith   (resources.at(0).Data, resourceEnd,   resourceEnd   + 8));
}