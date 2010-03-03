#pragma once

#include "INote.h"
#include "Timestamp.h"

class Note : public INote
{
private:

	Guid         guid;
	Timestamp    createDate;
	std::wstring title;

	boost::ptr_vector<ITag> tags;

public:

	Note(Guid guid, std::wstring title);

	virtual Guid GetGuid() const;

	virtual std::wstring GetTitle() const;

	virtual const boost::ptr_vector<ITag> & GetTags() const;

	virtual const ITimestamp & GetCreateDate() const;
};
