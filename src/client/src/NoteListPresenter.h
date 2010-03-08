#pragma once

#include "Note.h"
#include "Tag.h"
#include "Timestamp.h"
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

	static std::wstring ConvertToHtml(const Note & note);

	static std::wstring FormatTitle(const std::wstring & title);

	static std::wstring FormatTags(const TagList & tags);

	static std::wstring FormatDate(const Timestamp & timestamp);

	static std::wstring EscapeHtml(const std::wstring & string);
};
