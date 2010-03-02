#pragma once

#include "INote.h"
#include "MockTag.h"
#include "MockTimestamp.h"

class MockNote : public INote
{
public:

	Guid          guid;
	std::wstring  title;
	MockTimestamp createDate;

	boost::ptr_vector<ITag> tags;

public:

	virtual Guid GetGuid() const;

	virtual std::wstring GetTitle() const;

	virtual const boost::ptr_vector<ITag> & GetTags() const;

	virtual const ITimestamp & GetCreateDate() const;
};
