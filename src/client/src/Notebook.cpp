#include "stdafx.h"
#include "Notebook.h"

#include "Tools.h"

using namespace std;
using namespace Tools;

Notebook::Notebook(Guid guid, wstring name)
	: guid    (guid)
	, name    (name)
	, usn     (-1)
	, isDirty (true)
{
}

wstring Notebook::GetName() const
{
	return name;
}

Guid Notebook::GetGuid() const
{
	return guid;
}

int Notebook::GetUsn() const
{
	return usn;
}

bool Notebook::IsDirty() const
{
	return isDirty;
}
