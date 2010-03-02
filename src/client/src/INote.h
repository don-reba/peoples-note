#pragma once

#include "Guid.h"
#include "ITag.h"

class ITimestamp;

class INote
{
public:

	virtual ~INote() {}

	virtual Guid GetGuid() const = 0;

	virtual std::wstring GetTitle() const = 0;

	virtual const boost::ptr_vector<ITag> & GetTags() const = 0;

	virtual const ITimestamp & GetCreateDate() const = 0;
};
