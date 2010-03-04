#include "stdafx.h"
#include "MockEnImporter.h"

using namespace boost;
using namespace std;

void MockEnImporter::ImportNotes(wstring path, ptr_vector<INote> & notes)
{
	this->path = path;
	foreach (MockNote & note, this->notes)
		notes.push_back(new MockNote(note));
}