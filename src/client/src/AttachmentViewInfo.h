#pragma once

#include "Guid.h"

class AttachmentViewInfo
{
public:
	std::wstring Text;
	std::wstring Icon;
	::Guid       Guid;
	bool         IsPlayable;

	AttachmentViewInfo
		( const ::Guid       & guid
		, const std::wstring & icon
		, const std::wstring & text
		, bool                 isPlayable
		)
		: Guid(guid), Icon(icon), Text(text), IsPlayable(isPlayable)
	{
	}
};

typedef std::vector<AttachmentViewInfo> AttachmentViewInfoList;
