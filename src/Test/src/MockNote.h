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

	std::vector<MockTag> tags;

public:

	MockNote();

	MockNote
		( Guid          guid
		, std::wstring  title
		, MockTimestamp creationDate
		);

	virtual Guid GetGuid() const;

	virtual std::wstring GetTitle() const;

	virtual std::vector<const ITag*> GetTags() const;

	virtual const ITimestamp & GetCreationDate() const;
};
