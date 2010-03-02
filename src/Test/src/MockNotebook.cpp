#include "stdafx.h"
#include "MockNotebook.h"

using namespace std;

MockNotebook::MockNotebook()
{
}

MockNotebook::MockNotebook(wstring name)
	: name (name)
{
}

MockNotebook::MockNotebook(const INotebook & notebook)
	: name (notebook.GetName())
{
}

wstring MockNotebook::GetName() const
{
	return name;
}

Guid MockNotebook::GetGuid() const
{
	return guid;
}
