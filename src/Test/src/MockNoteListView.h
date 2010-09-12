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
	struct NotebookRecord
	{
		std::wstring html;
		std::wstring value;
	};

public:

	std::vector<NoteRecord>     notes;
	std::vector<NotebookRecord> notebooks;

	std::set<std::string> updatedThumbnails;

	bool notesUpdated;
	bool notebooksUpdated;

	std::wstring searchString;
	Guid         selectedNotebookGuid;
	Guid         selectedNoteGuid;

	bool         hasEnexPath;
	std::wstring enexPath;

	double       progressFraction;
	std::wstring profileText;
	std::wstring signinText;
	std::wstring statusText;
	std::wstring syncText;
	std::wstring windowTitle;

	bool isPageDownVisible;
	bool isPageUpVisible;
	bool isSyncButtonShown;
	bool isSyncEnabled;

	signal SignalAbout;
	signal SignalCreated;
	signal SignalImport;
	signal SignalNewNote;
	signal SignalNotebookSelected;
	signal SignalOpenNote;
	signal SignalPageDown;
	signal SignalPageUp;
	signal SignalProfile;
	signal SignalSearch;
	signal SignalSignIn;
	signal SignalSync;

public:

	MockNoteListView();

	virtual void AddNote
		( const std::wstring & html
		, const std::wstring & value
		);

	virtual void AddNotebook
		( const std::wstring & html
		, const std::wstring & value
		);

	virtual void ClearNotebooks();

	virtual void ClearNotes();

	virtual void ConnectAbout(slot_type OnAbout);

	virtual void ConnectCreated(slot_type OnCreated);

	virtual void ConnectImport(slot_type OnImport);

	virtual void ConnectNewNote(slot_type OnNewNote);

	virtual void ConnectNotebookSelected(slot_type OnNotebookSelected);

	virtual void ConnectOpenNote(slot_type OnOpenNote);

	virtual void ConnectPageDown(slot_type OnPageDown);

	virtual void ConnectPageUp(slot_type OnPageUp);

	virtual void ConnectProfile(slot_type OnProfile);

	virtual void ConnectSearch(slot_type OnSearch);

	virtual void ConnectSignIn(slot_type OnSignIn);

	virtual void ConnectSync(slot_type OnSync);

	virtual void DisableSync();

	virtual void EnableSync();

	virtual bool GetEnexPath(std::wstring & path);

	virtual Guid GetSelectedNotebookGuid();

	virtual Guid GetSelectedNoteGuid();
	
	virtual std::wstring GetSearchString();

	virtual void HidePageDown();

	virtual void HidePageUp();

	virtual void HideSyncButton();

	virtual void SetProfileText(const std::wstring & text);

	virtual void SetProgress(double fraction);

	virtual void SetSigninText(const std::wstring & text);

	virtual void SetStatusText(const std::wstring & text);

	virtual void SetSyncText(const std::wstring & text);

	virtual void SetWindowTitle(const std::wstring & text);

	virtual void ShowPageDown();

	virtual void ShowPageUp();

	virtual void ShowSyncButton();

	virtual void UpdateNotebooks();

	virtual void UpdateNotes();

	virtual void UpdateThumbnail(const Guid & guid);
};
