#pragma once

#include "INoteListModel.h"
#include "Note.h"

class IRegistryKey;
class IUserModel;

class NoteListModel : public INoteListModel
{
private:

	IRegistryKey & registryKey;
	IUserModel   & userModel;

	const int pageSize;

	NoteList notes;

	int currentPage;

	signal SignalChanged;

public:

	NoteListModel
		( int            pageSize
		, IUserModel   & userModel
		, IRegistryKey & registryKey
		);

	virtual void ConnectChanged(slot_type OnReset);

	virtual void GetCurrentPage
		( NoteList::const_iterator & begin
		, NoteList::const_iterator & end
		);

	virtual bool GetNotebookTitleState();

	virtual bool HasNextNotes();

	virtual bool HasPreviousNotes();

	virtual void Reload();

	virtual void SelectNextPage();

	virtual void SelectPreviousPage();

	virtual void SetNotebookTitleState(bool isEnabled);

	virtual void SetNotes(const NoteList & notes);
};
