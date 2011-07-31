#pragma once

#include "IHtmlDataLoader.h"

class EnNoteTranslator;
class Guid;
class INoteListView;
class INoteView;
class IUserModel;
class Thumbnail;

class HtmlDataLoader : public IHtmlDataLoader
{
private:

	enum UriType
	{
		UriTypeHtml,
		UriTypeHttpImg,
		UriTypeResource,
		UriTypeThumbnail,
		UriTypeUnknown,
	};

private:

	const bool highRes;

	EnNoteTranslator & enNoteTranslator;
	IUserModel       & userModel;

	INoteListView * noteListView;
	INoteView     * noteView;

public:

	HtmlDataLoader
		( bool               highRes
		, EnNoteTranslator & enNoteTranslator
		, IUserModel       & userModel
		);

	virtual bool LoadFromUri(const wchar_t * uri, Blob & blob);

	void AttachViews(INoteListView & noteListView, INoteView & noteView);

private:

	UriType ClassifyHttpUri (const wchar_t * uri);
	UriType ClassifyUri     (const wchar_t * uri);

	void CreateThumbnail(const Guid & guid, Thumbnail & thumbnail);

	bool IsPrefix
		( const wchar_t * begin
		, const wchar_t * end
		, const wchar_t * prefix
		);

	void LoadHtmlUri      (const wchar_t * uri, Blob & blob);
	void LoadHttpImgUri   (const wchar_t * uri, Blob & blob);
	void LoadResourceUri  (const wchar_t * uri, Blob & blob);
	void LoadThumbnailUri (const wchar_t * uri, Blob & blob);
};
