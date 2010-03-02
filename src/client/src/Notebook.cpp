#include "stdafx.h"
#include "Notebook.h"

#include "Tools.h"

using namespace std;
using namespace Tools;

Notebook::Notebook(Guid guid, wstring name)
	: guid (guid)
	, name (name)
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
