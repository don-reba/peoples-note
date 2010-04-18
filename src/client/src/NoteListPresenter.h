#pragma once

#include "EnNoteTranslator.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "INoteView.h"
#include "IUserModel.h"
#include "Note.h"
#include "Tag.h"
#include "Thumbnail.h"
#include "Timestamp.h"

class NoteListPresenter
{
private:

	INoteListModel   & noteListModel;
	INoteListView    & noteListView;
	INoteView        & noteView;
	IUserModel       & userModel;
	EnNoteTranslator & enNoteTranslator;

	Thumbnail thumbnail;

public:

	NoteListPresenter
		( INoteListModel   & noteListModel
		, INoteListView    & noteListView
		, INoteView        & noteView
		, IUserModel       & userModel
		, EnNoteTranslator & enNoteTranslator
		);

// event handlers

private:

	void OnLoadThumbnail(const Guid & guid, Blob *& blob);

	void OnNoteListChanged();

	void OnUserLoaded();

// utility functions

private:

	static std::wstring ConvertToHtml(const Note & note, const std::wstring & guid);

	static std::wstring FormatTitle(const std::wstring & title);

	static std::wstring FormatTags(const TagList & tags);

	static std::wstring FormatDate(const Timestamp & timestamp);

	static std::wstring EscapeHtml(const std::wstring & string);
};
