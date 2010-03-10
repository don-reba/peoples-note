#pragma once

#include "ISignalProvider.h"
#include "Guid.h"

class INoteListView : public ISignalProvider
{
public:

	virtual ~INoteListView() {}

	virtual void AddNote(std::wstring noteHtml, std::wstring value) = 0;

	virtual void AddNotebook(std::wstring notebook) = 0;

	virtual void ConnectCreated(slot_type OnCreated) = 0;

	virtual void ConnectImport(slot_type OnImport) = 0;

	virtual void ConnectOpenNote(slot_type OnOpenNote) = 0;

	virtual void ConnectSearch(slot_type OnSearch) = 0;

	virtual void ClearNotes() = 0;

	virtual void ClearNotebooks() = 0;

	virtual bool GetEnexPath(std::wstring & path) = 0;

	virtual Guid GetSelectedNoteGuid() = 0;

	virtual std::wstring GetSearchString() = 0;

	virtual void ShowNoteView() = 0;

	virtual void UpdateNotes() = 0;

	virtual void UpdateNotebooks() = 0;
};
