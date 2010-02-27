#include "stdafx.h"
#include "MockNotebook.h"

using namespace std;

MockNotebook::MockNotebook()
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
