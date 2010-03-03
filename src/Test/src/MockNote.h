#pragma once

#include "INote.h"
#include "MockTag.h"
#include "MockTimestamp.h"

class MockNote : public INote
{
public:

	Guid          guid;
	std::wstring  title;
	MockTimestamp creationDate;

	boost::ptr_vector<ITag> tags;

public:

	MockNote();

	MockNote
		( Guid          guid
		, std::wstring  title
		, MockTimestamp creationDate
		);

	virtual Guid GetGuid() const;

	virtual std::wstring GetTitle() const;

	virtual const boost::ptr_vector<ITag> & GetTags() const;

	virtual const ITimestamp & GetCreationDate() const;
};
