#include "stdafx.h"
#include "MockEnImporter.h"

using namespace boost;
using namespace std;

void MockEnImporter::ImportNotes
	( wistream          & stream
	, ptr_vector<INote> & notes
	)
{
	foreach (MockNote & note, this->notes)
		notes.push_back(new MockNote(note));
}
