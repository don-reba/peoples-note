#pragma once

#include "Note.h"
#include "Tag.h"
#include "Timestamp.h"
#include "INoteListModel.h"
#include "INoteListView.h"
#include "INoteView.h"
#include "IUserModel.h"

class NoteListPresenter
{
private:

	INoteListModel & noteListModel;
	INoteListView  & noteListView;
	INoteView      & noteView;
	IUserModel     & userModel;

	std::vector<HBITMAP> bitmaps;

public:

	NoteListPresenter
		( INoteListModel & noteListModel
		, INoteListView  & noteListView
		, INoteView      & noteView
		, IUserModel     & userModel
		);

private:

	void OnLoadBitmap(size_t previewIndex, HBITMAP & bmp);

	void OnNoteListChanged();

	void OnUserLoaded();

private:

	static std::wstring ConvertToHtml(const Note & note, int previewIndex);

	static std::wstring FormatTitle(const std::wstring & title);

	static std::wstring FormatTags(const TagList & tags);

	static std::wstring FormatDate(const Timestamp & timestamp);

	static std::wstring EscapeHtml(const std::wstring & string);
};
