#pragma once

class INoteListView
{
public:

	virtual void ClearNotes() = 0;

	virtual void AddNote(std::wstring note) = 0;

	virtual void Update() = 0;
};
