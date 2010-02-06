#include "stdafx.h"
#include "MockNotebook.h"

using namespace std;

wstring MockNotebook::GetName() const
{
	return name;
}

vector<INote*> MockNotebook::GetNotes() const
{
	return notes;
}
