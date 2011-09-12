#include "stdafx.h"
#include "MockFlashCard.h"

using namespace std;

bool MockFlashCard::Exists() const
{
	return !path.empty();
}

wstring MockFlashCard::GetPath() const
{
	return path;
}
