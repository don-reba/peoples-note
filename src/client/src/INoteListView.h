#pragma once

class INoteListView
{
public:

	virtual void ClearNotes() = 0;

	virtual void AddNote(std::wstring note) = 0;

	virtual void UpdateNotes() = 0;

	virtual void ClearNotebooks() = 0;

	virtual void AddNotebook(std::wstring notebook) = 0;

	virtual void UpdateNotebooks() = 0;
};
