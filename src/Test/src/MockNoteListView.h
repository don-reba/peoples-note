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

	signal SignalCreated;
	signal SignalImport;
	signal SignalOpenNote;
	signal SignalSearch;

public:

	MockNoteListView();

	virtual void AddNote(std::wstring noteHtml, std::wstring value);

	virtual void AddNotebook(std::wstring notebook);

	virtual void ClearNotebooks();

	virtual void ClearNotes();

	virtual void ConnectCreated(slot_type OnCreated);

	virtual void ConnectImport(slot_type OnImport);

	virtual void ConnectOpenNote(slot_type OnOpenNote);

	virtual void ConnectSearch(slot_type OnSearch);

	virtual bool GetEnexPath(std::wstring & path);

	virtual Guid GetSelectedNoteGuid();
	
	virtual std::wstring GetSearchString();

	virtual void UpdateNotebooks();

	virtual void UpdateNotes();
};
