#pragma once

#include "Guid.h"

class AttachmentViewInfo
{
public:
	std::wstring Text;
	std::wstring Icon;
	::Guid       Guid;

	AttachmentViewInfo
		( const ::Guid       & guid
		, const std::wstring & icon
		, const std::wstring & text)
		: Guid(guid), Icon(icon), Text(text)
	{
	}
};

typedef std::vector<AttachmentViewInfo> AttachmentViewInfoList;
