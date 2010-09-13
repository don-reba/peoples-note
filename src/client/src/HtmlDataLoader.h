#pragma once

#include "Blob.h"
#include "IHtmlDataLoader.h"

class EnNoteTranslator;
class INoteView;
class IUserModel;

class HtmlDataLoader : public IHtmlDataLoader
{
private:

	enum UriType
	{
		UriTypeHtml,
		UriTypeHttp,
		UriTypeResource,
		UriTypeThumbnail,
		UriTypeUnknown,
	};

private:

	Blob blob;

	EnNoteTranslator & enNoteTranslator;
	INoteView        & noteView;
	IUserModel       & userModel;

public:

	HtmlDataLoader
		( EnNoteTranslator & enNoteTranslator
		, INoteView        & noteView
		, IUserModel       & userModel
		);

	virtual BYTE * GetData();

	virtual DWORD GetDataSize();

	virtual bool LoadFromUri(const wchar_t * uri);

private:

	UriType ClassifyUri(const wchar_t * uri);

	bool IsPrefix
		( const wchar_t * begin
		, const wchar_t * end
		, const wchar_t * prefix
		);

	void LoadHtmlUri      (const wchar_t * uri);
	void LoadHttpUri      (const wchar_t * uri);
	void LoadResourceUri  (const wchar_t * uri);
	void LoadThumbnailUri (const wchar_t * uri);
};
