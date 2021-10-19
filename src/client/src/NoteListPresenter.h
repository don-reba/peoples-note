#pragma once

#include "EnNoteTranslator.h"
#include "Note.h"
#include "Tag.h"
#include "Thumbnail.h"
#include "Timestamp.h"

class INoteListModel;
class INoteListView;
class INoteView;
class ISyncModel;
class IUserModel;

class NoteListPresenter
{
private:

	INoteListModel   & noteListModel;
	INoteListView    & noteListView;
	INoteView        & noteView;
	ISyncModel       & syncModel;
	IUserModel       & userModel;
	EnNoteTranslator & enNoteTranslator;

	Thumbnail thumbnail;

public:

	NoteListPresenter
		( INoteListModel   & noteListModel
		, INoteListView    & noteListView
		, INoteView        & noteView
		, IUserModel       & userModel
		, ISyncModel       & syncModel
		, EnNoteTranslator & enNoteTranslator
		);

// event handlers

private:

	void OnNoteChanged();
	void OnNotesChanged();
	void OnNotebooksChanged();
	void OnTagsChanged();
	void OnNotebookSelected();
	void OnNotebookTitleChanged();
	void OnNoteListChanged();
	void OnPageDown();
	void OnPageUp();
	void OnSyncBegin();
	void OnSyncEnd();
	void OnSyncStatusUpdated();
	void OnUserLoaded();
	void OnViewStyleChanged();

// utility functions

private:

	void ResetSearch();

	void UpdateActiveNotebook();

	void UpdateNotebookListView();

	void UpdateNotebookTitle();

	void UpdateSyncCounter();

	void UpdateTitle();

	void UpdateViewStyle();

	static std::wstring ConvertToHtml(const Note & note, const std::wstring & guid);

	static std::wstring FormatTitle(const std::wstring & title);

	static std::wstring FormatTags(const TagList & tags);

	static std::wstring FormatDate(const Timestamp & timestamp);
};
