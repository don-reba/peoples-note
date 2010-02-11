#include "stdafx.h"
#include "Notebook.h"

using namespace std;

Notebook::Notebook()
{
	// TODO: implement
	notes.resize(4);
}

wstring Notebook::GetName() const
{
	// TODO: implement
	return L"test-notebook";
}

vector<INote*> Notebook::GetNotes() const
{
	vector<INote*> notes;
	notes.reserve(this->notes.size());
	foreach (Note & note, this->notes)
		notes.push_back(&note);
	return notes;
}
