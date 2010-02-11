#pragma once

#include "INote.h"
#include "Timestamp.h"

class Note : public INote
{
private:

	Timestamp    createDate;
	std::wstring title;

	std::vector<boost::shared_ptr<ITag> > tags;

public:

	virtual std::wstring GetTitle() const;

	virtual std::vector<const ITag*> GetTags() const;

	virtual const ITimestamp & GetCreateDate() const;
};
