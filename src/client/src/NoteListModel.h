#pragma once

#include "INoteListModel.h"
#include "Note.h"

class IRegistryKey;
class IUserModel;

class NoteListModel : public INoteListModel
{
	MacroEvent(Changed)

private:

	IRegistryKey & registryKey;
	IUserModel   & userModel;

	const size_t pageSize;

	bool hasNextPage;
	int currentPage;

	std::wstring query;

public:

	NoteListModel
		( int            pageSize
		, IUserModel   & userModel
		, IRegistryKey & registryKey
		);

	virtual void GetCurrentPage(NoteList & notes);

	virtual bool GetNotebookTitleState();

	virtual bool HasNextPage();

	virtual bool HasPreviousPage();

	virtual void Reload();

	virtual void SelectNextPage();

	virtual void SelectPreviousPage();

	virtual void SetNotebookTitleState(bool isEnabled);

	virtual void SetQuery(const std::wstring & query);
};
