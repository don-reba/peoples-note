#include "stdafx.h"
#include "MockSqlBlob.h"

#include <algorithm>
#include <iterator>

using namespace std;

int MockSqlBlob::GetSize()
{
	return data.size();
}

void MockSqlBlob::Read(Blob & data)
{
	data.assign(this->data.begin(), this->data.end());
}

void MockSqlBlob::Read(int offset, int size, Blob & data)
{
	if (offset < 0)
		throw invalid_argument("offset");
	if (size < 0)
		throw invalid_argument("size");
	if (static_cast<size_t>(offset) >= data.size())
		throw invalid_argument("offset");
	if (static_cast<size_t>(offset + size) > data.size())
		throw invalid_argument("size");
	data.clear();
	copy
		( this->data.begin() + offset
		, this->data.begin() + offset + size
		, back_inserter(data)
		);
}
