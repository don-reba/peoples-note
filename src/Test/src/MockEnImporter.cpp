#include "stdafx.h"
#include "MockEnImporter.h"

using namespace boost;
using namespace std;

void MockEnImporter::ImportNotes
	( wistream     & stream
	, NoteList     & notes
	, NoteBodyList & bodies
	, ImageList    & images
	)
{
	bodies = this->bodies;
	notes  = this->notes;
	images = this->images;
}
