#pragma once

#include "INoteListView.h"

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

	bool notesUpdated;
	bool notebooksUpdated;

	std::wstring searchString;
	Guid         selectedNotebookGuid;
	Guid         selectedNoteGuid;

	bool         hasEnexPath;
	std::wstring enexPath;

	std::wstring profileText;
	std::wstring signinText;
	std::wstring syncText;

	bool isSyncButtonShown;
	bool isSyncEnabled;

	signal SignalCreated;
	signal SignalImport;
	signal SignalNotebookSelected;
	signal SignalOpenNote;
	signal SignalSearch;
	signal SignalSignIn;
	signal SignalSync;

	DataSignal SignalLoadThumbnail;

public:

	MockNoteListView();

	virtual void AddNote(std::wstring html, std::wstring value);

	virtual void AddNotebook(std::wstring html, std::wstring value);

	virtual void ClearNotebooks();

	virtual void ClearNotes();

	virtual void ConnectCreated(slot_type OnCreated);

	virtual void ConnectImport(slot_type OnImport);

	virtual void ConnectLoadThumbnail(DataSlot OnLoadThumbnail);

	virtual void ConnectNotebookSelected(slot_type OnNotebookSelected);

	virtual void ConnectOpenNote(slot_type OnOpenNote);

	virtual void ConnectSearch(slot_type OnSearch);

	virtual void ConnectSignIn(slot_type OnSignIn);

	virtual void ConnectSync(slot_type OnSync);

	virtual void DisableSync();

	virtual void EnableSync();

	virtual bool GetEnexPath(std::wstring & path);

	virtual Guid GetSelectedNotebookGuid();

	virtual Guid GetSelectedNoteGuid();
	
	virtual std::wstring GetSearchString();

	virtual void HideSyncButton();

	virtual void SetProfileText(const std::wstring & text);

	virtual void SetSigninText(const std::wstring & text);

	virtual void SetSyncText(const std::wstring & text);

	virtual void ShowSyncButton();

	virtual void UpdateNotebooks();

	virtual void UpdateNotes();
};
