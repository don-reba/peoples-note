#include "stdafx.h"
#include "EnInteropNote.h"

using namespace std;

Guid EnInteropNote::GetGuid() const
{
	return note.GetGuid();
}

wstring EnInteropNote::GetName() const
{
	return note.GetName();
}

int EnInteropNote::GetUsn() const
{
	return note.GetUsn();
}

bool EnInteropNote::IsDirty() const
{
	return note.IsDirty();
}
