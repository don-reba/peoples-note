#include "stdafx.h"
#include "MockEnImporter.h"

using namespace boost;
using namespace std;

void MockEnImporter::ImportNotes
	( const std::wstring & text
	, NoteList           & notes
	, NoteBodyList       & bodies
	, ResourceList       & resources
	)
{
	this->text = text;
	bodies    = this->bodies;
	notes     = this->notes;
	resources = this->resources;
}
