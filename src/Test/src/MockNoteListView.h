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

public:

	std::vector<NoteRecord>   notes;
	std::vector<std::wstring> notebooks;

	bool notesUpdated;
	bool notebooksUpdated;

	std::wstring searchString;
	Guid         selectedNoteGuid;

	bool         hasEnexPath;
	std::wstring enexPath;

	std::wstring profileText;
	std::wstring signinText;

	bool isSyncButtonShown;

	signal SignalCreated;
	signal SignalImport;
	signal SignalOpenNote;
	signal SignalSearch;
	signal SignalSignIn;
	signal SignalSync;

	DataSignal SignalLoadThumbnail;

public:

	MockNoteListView();

	virtual void AddNote(std::wstring html, std::wstring value);

	virtual void AddNotebook(std::wstring html);

	virtual void ClearNotebooks();

	virtual void ClearNotes();

	virtual void ConnectCreated(slot_type OnCreated);

	virtual void ConnectImport(slot_type OnImport);

	virtual void ConnectLoadThumbnail(DataSlot OnLoadThumbnail);

	virtual void ConnectOpenNote(slot_type OnOpenNote);

	virtual void ConnectSearch(slot_type OnSearch);

	virtual void ConnectSignIn(slot_type OnSignIn);

	virtual void ConnectSync(slot_type OnSync);

	virtual bool GetEnexPath(std::wstring & path);

	virtual Guid GetSelectedNoteGuid();
	
	virtual std::wstring GetSearchString();

	virtual void HideSyncButton();

	virtual void SetProfileText(const std::wstring & text);

	virtual void SetSigninText(const std::wstring & text);

	virtual void ShowSyncButton();

	virtual void UpdateNotebooks();

	virtual void UpdateNotes();
};
