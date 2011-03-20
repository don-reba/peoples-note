#include "stdafx.h"
#include "MockFile.h"

#include <algorithm>
#include <iterator>

using namespace std;

bool MockFile::Read(const wstring &path, Blob & data)
{
	if (!exists)
		return false;

	data.clear();
	copy(this->data.begin(), this->data.end(), back_inserter(data));

	return true;
}

bool MockFile::Write(const wstring & path, const Blob & data)
{
	if (!exists)
		return false;

	this->data.clear();
	copy(data.begin(), data.end(), back_inserter(this->data));

	return true;
}