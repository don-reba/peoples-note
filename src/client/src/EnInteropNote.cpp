#include "stdafx.h"
#include "EnInteropNote.h"

EnInteropNote::EnInteropNote(const Note & note, const Guid & notebook)
	: guid     (note.guid)
	, note     (note)
	, notebook (notebook)
	, isDirty  (note.isDirty)
	, name     (note.name)
	, usn      (note.usn)
{
}
