#include "stdafx.h"
#include "MockHtmlDataLoader.h"

IHtmlDataLoader::UriType MockHtmlDataLoader::LoadFromUri(const wchar_t * uri, Blob & blob)
{
	return UriTypeUnknown;
}
