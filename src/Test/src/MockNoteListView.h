#pragma once

#include "Guid.h"
#include "INoteListView.h"

#include <set>

class MockNoteListView : public INoteListView
{
public:

	struct NoteRecord
	{
		std::wstring html;
		std::wstring value;
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

	bool isPageDownVisible;
	bool isPageUpVisible;
	bool isSyncButtonShown;
	bool isSyncEnabled;
	bool isSearchButtonSetToSearch;

	signal SignalAbout;
	signal SignalClearSearch;
	signal SignalCreated;
	signal SignalImport;
	signal SignalNewInkNote;
	signal SignalNewNote;
	signal SignalNewPhotoNote;
	signal SignalNotebookSelected;
	signal SignalOpenNote;
	signal SignalPageDown;
	signal SignalPageUp;
	signal SignalProfile;
	signal SignalSearch;
	signal SignalSearchChanged;
	signal SignalSignIn;
	signal SignalSync;

public:

	MockNoteListView();

	virtual void AddNote
		( const std::wstring & html
		, const std::wstring & value
		);

	virtual void ClearNotes();

	virtual void ConnectAbout(slot_type OnAbout);

	virtual void ConnectClearSearch(slot_type OnClearSearch);

	virtual void ConnectCreated(slot_type OnCreated);

	virtual void ConnectImport(slot_type OnImport);

	virtual void ConnectNewInkNote(slot_type OnNewInkNote);

	virtual void ConnectNewNote(slot_type OnNewNote);

	virtual void ConnectNewPhotoNote(slot_type OnNewPhoto);

	virtual void ConnectNotebookSelected(slot_type OnNotebookSelected);

	virtual void ConnectOpenNote(slot_type OnOpenNote);

	virtual void ConnectPageDown(slot_type OnPageDown);

	virtual void ConnectPageUp(slot_type OnPageUp);

	virtual void ConnectProfile(slot_type OnProfile);

	virtual void ConnectSearch(slot_type OnSearch);

	virtual void ConnectSearchChanged(slot_type OnSearchChanged);

	virtual void ConnectSignIn(slot_type OnSignIn);

	virtual void ConnectSync(slot_type OnSync);

	virtual void DisableSync();

	virtual void EnableSync();

	virtual bool GetEnexPath(std::wstring & path);

	virtual Guid GetSelectedNotebookGuid();

	virtual Guid GetSelectedNoteGuid();
	
	virtual std::wstring GetSearchString();

	virtual void GetThumbSize(SIZE & size);

	virtual void HidePageDown();

	virtual void HidePageUp();

	virtual void HideSyncButton();

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

	virtual void ShowPageDown();

	virtual void ShowPageUp();

	virtual void ShowSyncButton();

	virtual void UpdateNotes();

	virtual void UpdateThumbnail(const Guid & guid);
};
