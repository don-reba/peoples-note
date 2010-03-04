#pragma once

#include "INoteListView.h"

class MockNoteListView : public INoteListView
{
public:

	std::vector<std::wstring> notes;
	std::vector<std::wstring> notebooks;
	bool                      notesUpdated;
	bool                      notebooksUpdated;
	std::wstring              searchString;

	bool         hasEnexPath;
	std::wstring enexPath;

	signal SignalCreated;
	signal SignalImport;
	signal SignalSearch;

public:

	MockNoteListView();

	virtual void AddNote(std::wstring noteHtml);

	virtual void AddNotebook(std::wstring notebook);

	virtual void ClearNotebooks();

	virtual void ClearNotes();

	virtual void ConnectCreated(slot_type OnCreated);

	virtual void ConnectImport(slot_type OnImport);

	virtual void ConnectSearch(slot_type OnSearch);

	virtual bool GetEnexPath(std::wstring & path);
	
	virtual std::wstring GetSearchString();

	virtual void UpdateNotebooks();

	virtual void UpdateNotes();
};
