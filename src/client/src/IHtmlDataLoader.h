#pragma once

#include "Blob.h"

class IHtmlDataLoader
{
public:

	enum UriType
	{
		UriTypeHtml,
		UriTypeHttpImg,
		UriTypeResource,
		UriTypeThumbnail,
		UriTypeUnknown,
	};

public:

	virtual UriType LoadFromUri(const wchar_t * uri, Blob & blob) = 0;
};
