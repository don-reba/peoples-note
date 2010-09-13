#include "stdafx.h"
#include "HtmlDataLoader.h"

#include "EnNoteTranslator.h"
#include "HtmlResource.h"
#include "INoteView.h"
#include "Tools.h"
#include "Transaction.h"
#include "IUserModel.h"

using namespace std;
using namespace Tools;

HtmlDataLoader::HtmlDataLoader
	( EnNoteTranslator & enNoteTranslator
	, INoteView        & noteView
	, IUserModel       & userModel
	)
	: enNoteTranslator (enNoteTranslator)
	, noteView         (noteView)
	, userModel        (userModel)
{
}

BYTE * HtmlDataLoader::GetData()
{
	if (blob.empty())
		return NULL;
	return &blob[0];
}

DWORD HtmlDataLoader::GetDataSize()
{
	return blob.size();
}

bool HtmlDataLoader::LoadFromUri(const wchar_t * uri)
{
	try
	{
		switch (ClassifyUri(uri))
		{
		case UriTypeHtml:      LoadHtmlUri      (uri); return true;
		case UriTypeHttp:      LoadHttpUri      (uri); return true;
		case UriTypeResource:  LoadResourceUri  (uri); return true;
		case UriTypeThumbnail: LoadThumbnailUri (uri); return true;
		}
	}
	catch (const std::exception & e)
	{
		DEBUGMSG(true, (L"%s\n", ConvertToUnicode(e.what()).c_str()));
	}
	return false;
}

HtmlDataLoader::UriType HtmlDataLoader::ClassifyUri(const wchar_t * uri)
{
	const wchar_t * colonPosition(wcschr(uri, L':'));
	if (!colonPosition)
		return UriTypeHtml;
	if (IsPrefix(uri, colonPosition, L"thumb"))
		return UriTypeThumbnail;
	if (IsPrefix(uri, colonPosition, L"img"))
		return UriTypeResource;
	if (IsPrefix(uri, colonPosition, L"http"))
		return UriTypeHttp;
	return UriTypeUnknown;
}

bool HtmlDataLoader::IsPrefix
	( const wchar_t * begin
	, const wchar_t * end
	, const wchar_t * prefix
	)
{
	while (*prefix)
	{
		if (begin == end)
			return false;
		if (*begin != *prefix)
			return false;
		++begin;
		++prefix;
	}
	return begin == end;
}

void HtmlDataLoader::LoadHtmlUri(const wchar_t * uri)
{
	HtmlResource resource(LoadHtmlResource(uri));
	blob.assign(resource.data, resource.data + resource.size);
}

void HtmlDataLoader::LoadHttpUri(const wchar_t * uri)
{
	blob.clear();

	SHELLEXECUTEINFO info = { sizeof(info) };
	info.fMask  = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
	info.lpVerb = L"open";
	info.lpFile = uri;
	::ShellExecuteEx(&info);
}

void HtmlDataLoader::LoadResourceUri(const wchar_t * uri)
{
	const wchar_t * colonPosition (wcschr(uri,               L':'));
	const wchar_t * dotPosition   (wcschr(colonPosition + 1, L'.'));
	wstring hash(colonPosition + 1, dotPosition - colonPosition - 1);
	userModel.GetResource(ConvertToAnsi(hash), blob);
}

void HtmlDataLoader::LoadThumbnailUri(const wchar_t * uri)
{
	Guid guid(wcschr(uri, L':') + 1);

	Transaction transaction(userModel);

	Thumbnail thumbnail;
	userModel.GetNoteThumbnail(guid, thumbnail);

	const SIZE size = { 164, 100 };
	if (thumbnail.Width != size.cx || thumbnail.Height != size.cy)
	{
		wstring body;
		userModel.GetNoteBody(guid, body);

		wstring html;
		enNoteTranslator.ConvertToHtml(body, html);

		Note note;
		noteView.SetNote(note, L"", L"", html);

		thumbnail.Width  = size.cx;
		thumbnail.Height = size.cy;
		noteView.Render(thumbnail);
		userModel.SetNoteThumbnail(guid, thumbnail);
	}
	blob.swap(thumbnail.Data);
}
