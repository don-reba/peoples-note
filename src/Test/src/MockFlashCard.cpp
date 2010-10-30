#include "stdafx.h"
#include "MockFlashCard.h"

using namespace std;

bool MockFlashCard::Exists() const
{
	return !path.empty();
}

bool MockFlashCard::GetPath(wstring & path) const
{
	path = this->path;
	return !path.empty();
}
