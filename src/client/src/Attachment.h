#pragma once

#include "Guid.h"

class Attachment
{
public:

	Guid         Guid;
	std::wstring Mime;
	std::wstring FileName;

};

typedef std::vector<Attachment> AttachmentList;
