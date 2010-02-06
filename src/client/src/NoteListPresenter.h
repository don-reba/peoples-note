#pragma once

#include "INote.h"
#include "ITag.h"
#include "ITimestamp.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "IUserModel.h"

class NoteListPresenter
{
private:

	INoteListModel & noteListModel;
	INoteListView  & noteListView;
	IUserModel     & userModel;

public:

	NoteListPresenter
		( INoteListModel & noteListModel
		, INoteListView  & noteListView
		, IUserModel     & userModel
		);

	std::wstring ConvertToHtml(const INote * note) const;

	std::wstring FormatTitle(const std::wstring & title) const;

	std::wstring FormatTags(const std::vector<const ITag*> & tags) const;

	std::wstring FormatDate(const ITimestamp & timestamp) const;

	std::wstring EscapeHtml(const std::wstring & string) const;

private:

	void OnNoteListChanged();

	void OnUserLoaded();
};
