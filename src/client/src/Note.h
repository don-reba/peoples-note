#pragma once

#include "INote.h"
#include "Timestamp.h"

class Note : public INote
{
private:

	Guid         guid;
	Timestamp    creationDate;
	std::wstring title;

	boost::ptr_vector<ITag> tags;

public:

	Note(Guid guid, std::wstring title);

	virtual Guid GetGuid() const;

	virtual std::wstring GetTitle() const;

	virtual std::vector<const ITag*> GetTags() const;

	virtual const ITimestamp & GetCreationDate() const;
};
