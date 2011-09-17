#include "stdafx.h"
#include "HtmlDataLoader.h"

#include "Blob.h"
#include "EnNoteTranslator.h"
#include "HtmlResource.h"
#include "INoteListView.h"
#include "INoteView.h"
#include "Tools.h"
#include "Transaction.h"
#include "IUserModel.h"

#include "htmlayout.h"

using namespace std;
using namespace Tools;

HtmlDataLoader::HtmlDataLoader
	( bool               highRes
	, EnNoteTranslator & enNoteTranslator
	, IUserModel       & userModel
	)
	: highRes          (highRes)
	, enNoteTranslator (enNoteTranslator)
	, userModel        (userModel)
{
}

bool HtmlDataLoader::LoadFromUri(const wchar_t * uri, Blob & blob)
{
	try
	{
		switch (ClassifyUri(uri))
		{
		case UriTypeHtml:      LoadHtmlUri      (uri, blob); return true;
		case UriTypeHttpImg:   LoadHttpImgUri   (uri, blob); return true;
		case UriTypeResource:  LoadResourceUri  (uri, blob); return true;
		case UriTypeThumbnail: LoadThumbnailUri (uri, blob); return true;
		}
	}
	catch (const std::exception & e)
	{
		DEBUGMSG(true, (L"%s\n", ConvertToUnicode(e.what()).c_str()));
	}
	return false;
}

void HtmlDataLoader::AttachViews(INoteListView & noteListView, INoteView & noteView)
{
	this->noteListView = &noteListView;
	this->noteView     = &noteView;
}

HtmlDataLoader::UriType HtmlDataLoader::ClassifyHttpUri(const wchar_t * uri)
{
	const wchar_t * dotPosition(wcsrchr(uri, L'.'));
	if (!dotPosition)
		return UriTypeUnknown;
	if (0 == wcsicmp(dotPosition, L".gif"))
		return UriTypeHttpImg;
	if (0 == wcsicmp(dotPosition, L".jpg"))
		return UriTypeHttpImg;
	if (0 == wcsicmp(dotPosition, L".jpeg"))
		return UriTypeHttpImg;
	if (0 == wcsicmp(dotPosition, L".png"))
		return UriTypeHttpImg;
	return UriTypeUnknown;
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
		return ClassifyHttpUri(uri);
	if (IsPrefix(uri, colonPosition, L"https"))
		return ClassifyHttpUri(uri);
	return UriTypeUnknown;
}

void HtmlDataLoader::CreateThumbnail(const Guid & guid, Thumbnail & thumbnail)
{
	try
	{
		wstring body;
		userModel.GetNoteBody(guid, body);

		wstring html;
		enNoteTranslator.ConvertToHtml(body, html);

		Note note;
		noteView->SetNote(note, L"", L"", html, AttachmentViewInfoList(), false);

		noteView->Render(thumbnail);
	}
	catch (...)
	{
		HtmlResource resource(LoadHtmlResource(L"thumbnail.png", highRes));
		thumbnail.Data.clear();
		thumbnail.Data.reserve(resource.size);
		copy(resource.data, resource.data + resource.size, back_inserter(thumbnail.Data));
	}
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

void HtmlDataLoader::LoadHtmlUri(const wchar_t * uri, Blob & blob)
{
	HtmlResource resource(LoadHtmlResource(uri, highRes));
	blob.assign(resource.data, resource.data + resource.size);
}

void HtmlDataLoader::LoadHttpImgUri(const wchar_t * uri, Blob & blob)
{
}

void HtmlDataLoader::LoadResourceUri(const wchar_t * uri, Blob & blob)
{
	const wchar_t * colonPosition (wcschr(uri,               L':'));
	const wchar_t * dotPosition   (wcschr(colonPosition + 1, L'.'));
	wstring hash(colonPosition + 1, dotPosition - colonPosition - 1);
	try
	{
		userModel.GetResource(ConvertToAnsi(hash), blob);
	}
	catch(const std::exception &)
	{
		blob.clear();
	}
}

void HtmlDataLoader::LoadThumbnailUri(const wchar_t * uri, Blob & blob)
{
	if (!noteListView || !noteView)
	{
		DEBUGMSG(true, (L"Attemted to load thumbnails before setting views."));
		return;
	}

	Guid guid(wcschr(uri, L':') + 1);

	Transaction transaction(userModel);

	Thumbnail thumbnail;
	userModel.GetNoteThumbnail(guid, thumbnail);

	SIZE size;
	noteListView->GetThumbSize(size);
	if (thumbnail.Data.empty() || thumbnail.Width != size.cx || thumbnail.Height != size.cy)
	{
		thumbnail.Width  = size.cx;
		thumbnail.Height = size.cy;
		CreateThumbnail(guid, thumbnail);
		userModel.SetNoteThumbnail(guid, thumbnail);
	}

	blob.swap(thumbnail.Data);
}
