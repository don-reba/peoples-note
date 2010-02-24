#include "stdafx.h"
#include "Notebook.h"

using namespace std;

Notebook::Notebook(wstring name)
	: name (name)
{
}

wstring Notebook::GetName() const
{
	return name;
}
