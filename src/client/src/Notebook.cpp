#include "stdafx.h"
#include "Notebook.h"

using namespace std;

wstring Notebook::GetName() const
{
	return name;
}

vector<INote*> Notebook::GetNotes() const
{
	vector<INote*> notes;
	notes.reserve(this->notes.size());
	foreach (Note & note, this->notes)
		notes.push_back(&note);
	return notes;
}
