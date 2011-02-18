#pragma once

#include "Guid.h"
#include "INoteListView.h"

#include <set>

class MockNoteListView : public INoteListView
{
	MacroTestEvent(About)
	MacroTestEvent(ClearSearch)
	MacroTestEvent(Import)
	MacroTestEvent(NewInkNote)
	MacroTestEvent(NewNote)
	MacroTestEvent(NewPhotoNote)
	MacroTestEvent(NewVoiceNote)
	MacroTestEvent(NotebookSelected)
	MacroTestEvent(NotebookTitle)
	MacroTestEvent(OpenNote)
	MacroTestEvent(PageDown)
	MacroTestEvent(PageUp)
	MacroTestEvent(Profile)
	MacroTestEvent(Search)
	MacroTestEvent(SearchChanged)
	MacroTestEvent(SignIn)
	MacroTestEvent(Sync)

public:

	struct NoteRecord
	{
		std::wstring html;
		std::wstring value;
		bool         isDirty;
	};

public:

	std::wstring notebookMenu;

	std::vector<NoteRecord> notes;

	std::set<std::string> updatedThumbnails;

	bool notesUpdated;

	std::wstring searchString;
	Guid         selectedNotebookGuid;
	Guid         selectedNoteGuid;

	bool         hasEnexPath;
	std::wstring enexPath;

	double       progressFraction;
	std::wstring profileText;
	std::wstring searchText;
	std::wstring signinText;
	std::wstring statusText;
	std::wstring syncText;
	std::wstring windowTitle;

	SIZE thumbSize;

	bool isNotebookTitleEnabled;
	bool isNotebookTitleVisible;
	bool isPageDownVisible;
	bool isPageUpVisible;
	bool isSyncButtonShown;
	bool isSyncEnabled;
	bool isSearchButtonSetToSearch;

public:

	MockNoteListView();

	virtual void AddNote
		( const std::wstring & html
		, const std::wstring & value
		,       bool           isDirty
		);

	virtual void CheckNotebookTitleOption();

	virtual void ClearNotes();

	virtual void DisableSync();

	virtual void EnableSync();

	virtual bool GetEnexPath(std::wstring & path);

	virtual Guid GetSelectedNotebookGuid();

	virtual Guid GetSelectedNoteGuid();
	
	virtual std::wstring GetSearchString();

	virtual void GetThumbSize(SIZE & size);

	virtual void HideNotebookTitle();

	virtual void HidePageDown();

	virtual void HidePageUp();

	virtual void HideSyncButton();

	virtual bool IsNotebookTitleOptionChecked();

	virtual void SetNotebookMenu(const std::wstring & html);

	virtual void SetProfileText(const std::wstring & text);

	virtual void SetProgress(double fraction);

	virtual void SetSearchButtonToClear();

	virtual void SetSearchButtonToSearch();

	virtual void SetSearchText(const std::wstring & text);

	virtual void SetSigninText(const std::wstring & text);

	virtual void SetStatusText(const std::wstring & text);

	virtual void SetSyncText(const std::wstring & text);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void ShowNotebookTitle();

	virtual void ShowPageDown();

	virtual void ShowPageUp();

	virtual void ShowSyncButton();

	virtual void UncheckNotebookTitleOption();

	virtual void UpdateNotes();

	virtual void UpdateThumbnail(const Guid & guid);
};
