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

private:

	void OnNoteListChanged();

	void OnUserLoaded();

private:

	static std::wstring ConvertToHtml(const INote * note);

	static std::wstring FormatTitle(const std::wstring & title);

	static std::wstring FormatTags(const std::vector<const ITag*> & tags);

	static std::wstring FormatDate(const ITimestamp & timestamp);

	static std::wstring EscapeHtml(const std::wstring & string);
};
