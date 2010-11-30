#pragma once

#include "Blob.h"
#include "IHtmlDataLoader.h"

class EnNoteTranslator;
class INoteListView;
class INoteView;
class IUserModel;

class HtmlDataLoader : public IHtmlDataLoader
{
private:

	enum UriType
	{
		UriTypeHtml,
		UriTypeHttpHtml,
		UriTypeHttpImg,
		UriTypeResource,
		UriTypeThumbnail,
		UriTypeUnknown,
	};

private:

	Blob blob;

	bool highRes;

	EnNoteTranslator & enNoteTranslator;
	INoteListView    & noteListView;
	INoteView        & noteView;
	IUserModel       & userModel;

public:

	HtmlDataLoader
		( bool               highRes
		, EnNoteTranslator & enNoteTranslator
		, INoteListView    & noteListView
		, INoteView        & noteView
		, IUserModel       & userModel
		);

	virtual BYTE * GetData();

	virtual DWORD GetDataSize();

	virtual bool LoadFromUri(const wchar_t * uri);

private:

	UriType ClassifyHttpUri (const wchar_t * uri);
	UriType ClassifyUri     (const wchar_t * uri);

	bool IsPrefix
		( const wchar_t * begin
		, const wchar_t * end
		, const wchar_t * prefix
		);

	void LoadHtmlUri      (const wchar_t * uri);
	void LoadHttpHtmlUri  (const wchar_t * uri);
	void LoadHttpImgUri   (const wchar_t * uri);
	void LoadResourceUri  (const wchar_t * uri);
	void LoadThumbnailUri (const wchar_t * uri);
};
