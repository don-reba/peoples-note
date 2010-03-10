#include "stdafx.h"
#include "MockEnImporter.h"

using namespace boost;
using namespace std;

void MockEnImporter::ImportNotes
	( wistream     & stream
	, NoteList     & notes
	, NoteBodyList & bodies
	)
{
	bodies = this->bodies;
	notes  = this->notes;
}
