#include "stdafx.h"
#include "MockNotebook.h"

using namespace std;

vector<INote*> MockNotebook::GetNotes()
{
	return notes;
}
