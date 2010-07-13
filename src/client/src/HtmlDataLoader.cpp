#include "stdafx.h"
#include "HtmlDataLoader.h"

void * HtmlDataLoader::GetData()
{
	if (blob.empty())
		return NULL;
	return &blob[0];
}

DWORD HtmlDataLoader::GetDataSize()
{
	return blob.size();
}

void HtmlDataLoader::LoadFromUri(const wchar_t * uri)
{
	//---------
	// relative
	//---------
	
	try
	{
		HtmlResource resource(LoadHtmlResource(params->uri));
		params->outData     = resource.data;
		params->outDataSize = resource.size;
		return LOAD_OK;
	}
	catch (const std::exception & e)
	{
		DEBUGMSG(true, (L"%s\n", ConvertToUnicode(e.what()).c_str()));
		return LOAD_DISCARD;
	}

	//---------
	// NoteView
	//---------

	if (NULL == wcschr(params->uri, L':'))
		return __super::OnLoadData(params);
	try
	{
		SignalLoadingData(params->uri, blob);
	}
	catch (const std::exception & e)
	{
		DEBUGMSG(true, (L"%s\n", ConvertToUnicode(e.what()).c_str()));
		return LOAD_DISCARD;
	}
	params->outData     = &blob[0];
	params->outDataSize = blob.size();
	return LOAD_OK;

	//-------------
	// NoteListView
	//-------------

	wstring prefix(L"thumb:");
	if (0 == wcsncmp(params->uri, prefix.c_str(), prefix.size()))
	{
		Guid guid(params->uri + prefix.size());
		Blob * blob(NULL);
		SignalLoadThumbnail(guid, blob);
		if (NULL == blob)
			return LOAD_DISCARD;
		params->outData     = &blob->at(0);
		params->outDataSize = blob->size();
		return LOAD_OK;
	}
	return __super::OnLoadData(params);
}
