#pragma once

#include "Attachment.h"
#include "AttachmentViewInfo.h"

class INoteListModel;
class INoteListView;
class INoteView;
class IUserModel;
class EnNoteTranslator;
class Note;

class NotePresenter
{
private:

	INoteListModel   & noteListModel;
	INoteListView    & noteListView;
	INoteView        & noteView;
	IUserModel       & userModel;
	EnNoteTranslator & enNoteTranslator;

// interface

public:

	NotePresenter
		( INoteListModel   & noteListModel
		, INoteListView    & noteListView
		, INoteView        & noteView
		, IUserModel       & userModel
		, EnNoteTranslator & enNoteTranslator
		);

// event handlers

private:

	void OnAttachment();
	void OnCloseNote();
	void OnOpenNote();
	void OnToggleMaximize();

// utility functions

	void CreateSubtitle(const Note & note, std::wstring & subtitle);

	static AttachmentViewInfo ConvertAttachment(const Attachment & attachment);

	static const wchar_t * GetAttachmentImageUrl(const Attachment & mime);
};
