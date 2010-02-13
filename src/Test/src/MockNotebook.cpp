#include "stdafx.h"
#include "MockNotebook.h"

using namespace std;

wstring MockNotebook::GetName() const
{
	return name;
}

void MockNotebook::SetName(wstring name)
{
	this->name = name;
}

vector<INote*> MockNotebook::GetNotes() const
{
	return notes;
}

vector<INote*> MockNotebook::GetNotes(wstring search) const
{
	this->search = search;
	return notes;
}
