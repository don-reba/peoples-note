#include "stdafx.h"
#include "Note.h"

using namespace std;

wstring Note::GetTitle() const
{
	// TODO: implement
	return L"test-title";
}

vector<const ITag*> Note::GetTags() const
{
	// TODO: implement
	return vector<const ITag*>();
}

const ITimestamp & Note::GetCreateDate() const
{
	return createDate;
}
